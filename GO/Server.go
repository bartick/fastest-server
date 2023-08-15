package main

import (
	"errors"
	"fmt"
	"io"
	"net/http"
	"os"
	"regexp"
)

type route struct {
	pattern *regexp.Regexp
	handler http.Handler
}

type RegexpHandler struct {
	routes []*route
}

func (h *RegexpHandler) HandleFunc(pattern *regexp.Regexp, handler func(http.ResponseWriter, *http.Request)) {
	h.routes = append(h.routes, &route{pattern, http.HandlerFunc(handler)})
}

func getRoot(w http.ResponseWriter, r *http.Request) {
	io.WriteString(w, "Hello World!\nRequest: "+r.URL.Path+"\n")
}

func main() {
	const anyPattern = `^/.*`

	routes := RegexpHandler{}
	routes.HandleFunc(regexp.MustCompile(anyPattern), getRoot)

	http.HandleFunc("/", getRoot)

	fmt.Printf("starting server on port 8080\n")

	err := http.ListenAndServe(":8080", nil)

	if errors.Is(err, http.ErrServerClosed) {
		fmt.Printf("server closed\n")
	} else if err != nil {
		fmt.Printf("error starting server: %s\n", err)
		os.Exit(1)
	}
}
