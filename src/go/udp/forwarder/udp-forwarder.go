/**
 * FileName:   udp-forwarder.go
 * Author:     Fasion Chan
 * @contact:   fasionchan@gmail.com
 * @version:   $Id$
 *
 * Description:
 *
 * Changelog:
 *
 **/

package main

import (
    "flag"
    "fmt"
    "net"
    "os"
    "strconv"
    "time"
)

type Options struct {
    RemoteIP string
    RemotePort int

    BindIP string
    ListenPort int

    SessionTimeout string
}

func parseOptions() (*Options) {
    var options Options

    flag.StringVar(&options.RemoteIP, "remote-ip", "", "")
    flag.IntVar(&options.RemotePort, "remote-port", 0, "")

    flag.StringVar(&options.BindIP, "bind-ip", "", "")
    flag.IntVar(&options.ListenPort, "listen-port", 0, "")
    flag.StringVar(&options.SessionTimeout, "session-timeout", "1m", "")

    flag.Parse()

    if options.RemoteIP == "" {
        fmt.Println("No remote ip")
        os.Exit(1)
    }

    if options.RemotePort == 0 {
        fmt.Println("No remote port")
        os.Exit(1)
    }

    if options.BindIP == "" {
        fmt.Println("No bind ip")
        os.Exit(1)
    }

    if options.ListenPort == 0 {
        fmt.Println("No listen port")
        os.Exit(1)
    }

    return &options
}

type UDPSession struct {
    Key string
    Forwarder *UDPForwarder

    SourceIP string
    SourcePort int
    SourcePair string

    RemoteIP string
    RemotePort int
    RemotePair string

    SourceAddress *net.UDPAddr
    RemoteAddress *net.UDPAddr

    SourceConnection *net.UDPConn
    RemoteConnection *net.UDPConn

    LastActiveTime time.Time
    Timeout time.Duration
}

func (session *UDPSession) ForwardForever() () {
    fmt.Printf("session routine [%s] started\n", session.Key)

    for {
        deadline := session.LastActiveTime.Add(session.Timeout)
        session.RemoteConnection.SetReadDeadline(deadline)

        buffer := make([]byte, 65536)
        n, err := session.RemoteConnection.Read(buffer)
        if err != nil {
            switch err := err.(type) {
            case net.Error:
                if err.Timeout() {
                    if time.Now().Sub(session.LastActiveTime) >= session.Timeout {
                        break
                    }

                    continue
                }
            }

            fmt.Printf("session [%s] read error: %s\n", session.Key, err)
            break
        }

        session.LastActiveTime = time.Now()

        fmt.Printf(
            "%21s <---- %-21s [%d]\n",
            session.SourcePair,
            session.RemotePair,
            n,
        )

        n, err = session.SourceConnection.WriteTo(
            buffer[:n],
            session.SourceAddress,
        )
        if err != nil {
            fmt.Printf("session [%s] write error: %s", session.Key, err)
            break
        }
    }

    fmt.Printf("session routine [%s] stop\n", session.Key)

    session.Forwarder.FeedTimeoutSession(session)
}

func (session *UDPSession) FeedData(data []byte) () {
    session.RemoteConnection.Write(data)
    session.LastActiveTime = time.Now()

    fmt.Printf(
        "%21s ----> %-21s [%d]\n",
        session.SourcePair,
        session.RemotePair,
        len(data),
    )
}

type SessionMapping map[string]*UDPSession

type UDPForwarder struct {
    RemoteIP string
    RemotePort int
    RemoteAddress *net.UDPAddr

    Listener *net.UDPConn

    Sessions SessionMapping

    SessionTimeout time.Duration
    TimeoutSessions chan *UDPSession
}

func (forwarder *UDPForwarder) FeedTimeoutSession(session *UDPSession) {
    forwarder.TimeoutSessions <- session
}

func (forwarder *UDPForwarder) ServeForever() {
    defer forwarder.Listener.Close()

    interval := time.Second * 1
    buffer := make([]byte, 65536)

    for {
        forwarder.Listener.SetReadDeadline(time.Now().Add(interval))
        n, sourceAddress, err := forwarder.Listener.ReadFromUDP(buffer)
        if err != nil {
            switch err := err.(type) {
            case net.Error:
                if err.Timeout() {
                    select {
                    case session := <-forwarder.TimeoutSessions:
                        delete(forwarder.Sessions, session.Key)

                        fmt.Printf(
                            "session [%s] destroy, sessions=%d\n",
                            session.Key,
                            len(forwarder.Sessions),
                        )

                    default:
                    }

                    continue
                }
            }

            fmt.Println("read error:", err)
            continue
        }

        session, err := forwarder.GetUDPSession(sourceAddress)

        session.FeedData(buffer[:n])
    }
}

func (forwarder *UDPForwarder) GetUDPSession(SourceAddress *net.UDPAddr) (*UDPSession, error) {

    key := SourceAddress.IP.String() + ":" + strconv.Itoa(SourceAddress.Port)

    value, ok := forwarder.Sessions[key]
    if ok {
        return value, nil
    }

    conn, err := net.DialUDP(
        "udp",
        &net.UDPAddr{IP: net.IPv4zero, Port: 0},
        forwarder.RemoteAddress,
    )
    if err != nil {
        return nil, err
    }

    session := &UDPSession{
        Key: key,
        Forwarder: forwarder,

        SourceIP: SourceAddress.IP.String(),
        SourcePort: SourceAddress.Port,
        SourcePair: SourceAddress.IP.String() + ":" + strconv.Itoa(SourceAddress.Port),

        RemoteIP: forwarder.RemoteIP,
        RemotePort: forwarder.RemotePort,
        RemotePair: forwarder.RemoteIP + ":" + strconv.Itoa(forwarder.RemotePort),

        SourceAddress: SourceAddress,
        RemoteAddress: forwarder.RemoteAddress,

        SourceConnection: forwarder.Listener,
        RemoteConnection: conn,

        LastActiveTime: time.Now(),
        Timeout: forwarder.SessionTimeout,
    }

    forwarder.Sessions[key] = session
    fmt.Printf(
        "session [%s] created, sessions=%d\n",
        key,
        len(forwarder.Sessions),
    )

    go session.ForwardForever()

    return session, nil
}

func CreateUDPForwarder(options *Options) (*UDPForwarder, error) {

    bindAddress := &net.UDPAddr{
        IP: net.ParseIP(options.BindIP),
        Port: options.ListenPort,
    }

    listener, err := net.ListenUDP("udp", bindAddress)
    if err != nil {
        return nil, err
    }

    sessionTimeout, err := time.ParseDuration(options.SessionTimeout)
    if err != nil {
        return nil, err
    }

    remoteAddress := &net.UDPAddr{
        IP: net.ParseIP(options.RemoteIP),
        Port: options.RemotePort,
    }

    forwarder := &UDPForwarder {
        RemoteIP: options.RemoteIP,
        RemotePort: options.RemotePort,
        RemoteAddress: remoteAddress,

        Listener: listener,
        Sessions: make(SessionMapping),

        SessionTimeout: sessionTimeout,
        TimeoutSessions: make(chan *UDPSession),
    }

    return forwarder, nil
}

func main() {
    options := parseOptions()

    forwarder, err := CreateUDPForwarder(options)
    if err != nil {
        fmt.Println("create forwarder error:", err)
        os.Exit(1)
    }

    forwarder.ServeForever()
}
