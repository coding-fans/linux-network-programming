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

    "github.com/emicklei/go-restful"
)

type DataItem struct {
    Key string
    Name string
    Population int
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

    return service
}

func Create(request *restful.Request, response *restful.Response) {
    var item DataItem
    err := request.ReadEntity(&item)
    if err != nil {
        return
    }

    mapping[item.Key] = &item

    response.WriteEntity(item)
}

func Delete(request *restful.Request, response *restful.Response) {
    key := request.PathParameter("key")

    item, ok := mapping[key]
    if ok {
        delete(mapping, key)
    }

    response.WriteEntity(item)
}

func Update(request *restful.Request, response *restful.Response) {
    key := request.PathParameter("key")


    item, ok := mapping[key]
    if ok {
        err := request.ReadEntity(&item)
        if err != nil {
            response.WriteEntity(nil)
        }
    }

    response.WriteEntity(item)
}

func Retrieve(request *restful.Request, response *restful.Response) {
    key := request.PathParameter("key")
    item, _ := mapping[key]
    response.WriteEntity(item)
}

func Search(request *restful.Request, response *restful.Response) {

}

var mapping  = map[string]*DataItem{}

func main() {
    fmt.Println("kvs service starting...")

    restful.Add(NewKVService())
    log.Fatal(http.ListenAndServe(":8080", nil))
}
