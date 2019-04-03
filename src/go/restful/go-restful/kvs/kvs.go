/**
 * FileName:   kvs.go
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
    "fmt"
    "net/http"
    "log"
    "strconv"

    "github.com/emicklei/go-restful"
)

const (
    SIZE_LIMIT = 100
)

type DataItem struct {
    Key string `json:"key"`
    Name string `json:"name"`
    Population int `json:"population"`
}

type ResultMeta struct {
    Total uint64 `json:"total"`
    Skip uint64 `json:"skip"`
    Count uint64 `json:"count"`
}

type Result struct {
    Result bool `json:"result"`
    Data interface{} `json:"data"`
    Meta *ResultMeta `json:"meta"`
    Message string `json:"message"`
}

func NewKVService() *restful.WebService {
    service := new(restful.WebService)
    service.
        Path("/kvs").
        Consumes(restful.MIME_JSON).
        Produces(restful.MIME_JSON)

    service.Route(service.POST("").To(Create))
    service.Route(service.DELETE("/{key}").To(Delete))
    service.Route(service.PUT("/{key}").To(Update))
    service.Route(service.GET("/{key}").To(Retrieve))
    service.Route(service.GET("").To(Search))

    return service
}

func Create(request *restful.Request, response *restful.Response) {
    if len(mapping) >= SIZE_LIMIT {
        response.WriteEntity(&Result{
            Result: false,
            Data: nil,
            Message: "out of resources",
        })
        return
    }

    var item DataItem
    err := request.ReadEntity(&item)
    if err != nil {
        response.WriteEntity(&Result{
            Result: false,
            Data: nil,
            Message: err.Error(),
        })
        return
    }

    if item.Key == "" {
        response.WriteEntity(&Result{
            Result: false,
            Data: nil,
            Message: "data key missing",
        })
        return
    }

    _, ok := mapping[item.Key]
    if ok {
        response.WriteEntity(&Result{
            Result: false,
            Data: nil,
            Message: "resource exists",
        })
        return
    }

    mapping[item.Key] = &item

    response.WriteEntity(&Result{
        Result: true,
        Data: &item,
    })
}

func Delete(request *restful.Request, response *restful.Response) {
    key := request.PathParameter("key")

    item, ok := mapping[key]
    if !ok {
        response.WriteEntity(&Result{
            Result: false,
            Message: "resource not exists",
        })

        return
    }

    delete(mapping, key)

    response.WriteEntity(&Result{
        Result: true,
        Data: item,
    })
}

func Update(request *restful.Request, response *restful.Response) {
    key := request.PathParameter("key")

    item, ok := mapping[key]
    if !ok {
        response.WriteEntity(&Result{
            Result: false,
            Message: "resource not exists",
        })

        return
    }

    err := request.ReadEntity(item)
    if err != nil {
        response.WriteEntity(&Result{
            Result: false,
            Message: err.Error(),
        })

        return
    }

    response.WriteEntity(&Result{
        Result: true,
        Data: item,
    })
}

func Retrieve(request *restful.Request, response *restful.Response) {
    key := request.PathParameter("key")
    item, ok := mapping[key]
    if !ok {
        response.WriteEntity(&Result{
            Result: false,
            Message: "resource not exists",
        })

        return
    }

    response.WriteEntity(&Result{
        Result: true,
        Data: item,
    })
}

func Search(request *restful.Request, response *restful.Response) {
    var skip uint64 = 0
    var limit uint64 = 10
    var err error

    arg_skip := request.QueryParameter("skip")
    if arg_skip != "" {
        skip, err = strconv.ParseUint(arg_skip, 10, 64)
        if err != nil {
            response.WriteEntity(&Result{
                Result: false,
                Message: err.Error(),
            })
            return
        }
    }

    arg_limit := request.QueryParameter("limit")
    if arg_limit != "" {
        limit, err = strconv.ParseUint(arg_limit, 10, 64)
        if err != nil {
            response.WriteEntity(&Result{
                Result: false,
                Message: err.Error(),
            })
            return
        }
    }

    items := make([]*DataItem, 0, limit)

    var i uint64 = 0
    for _, item := range mapping {
        if i < skip {
            i += 1
            continue
        }

        items = append(items, item)
        if uint64(len(items)) >= limit {
            break
        }
    }

    response.WriteEntity(&Result{
        Result: true,
        Data: items,
        Meta: &ResultMeta{
            Total: uint64(len(mapping)),
            Skip: skip,
            Count: uint64(len(items)),
        },
    })
}

var mapping  = map[string]*DataItem{}

func main() {
    fmt.Println("kvs service starting...")

    restful.Add(NewKVService())
    log.Fatal(http.ListenAndServe(":8080", nil))
}
