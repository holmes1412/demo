package main

import (
	"context"
	"fmt"
	"time"
)

func async(ctx context.Context) chan error {
	var ch = make(chan error)

	go func() {
		var err error
		fmt.Println("start async calculation")
		defer func() {
			ch <- err
			close(ch)
		} ()

		time.Sleep(2 * time.Second)
		fmt.Println("end async calculation")
	} ()
	return ch
}

func impl1(ctx context.Context) {
	e := func() error {
		select {
		case <-ctx.Done():
			return ctx.Err()
		case err := <-async(ctx):
			return err
		}
	}()

	fmt.Print("finish impl1(): ", e, "\n")
}

func impl2(ctx context.Context) {
	e := func() error {
		fmt.Println("start sync calculation")

		timeout := 1 * time.Second
		deadline, ok := ctx.Deadline()
		if !ok || deadline.After(time.Now().Add(timeout)) {
			_ctx, cancel := context.WithTimeout(ctx, timeout)
			ctx = _ctx
			defer cancel()
		}
		time.Sleep(2 * time.Second)
		fmt.Println("end sync calculation")
		return nil
	}()

	fmt.Print("finish impl2(): ", e, "\n")
}

func main() {
	ctx, cancel := context.WithTimeout(context.Background(), 1*time.Second)
	defer cancel()

//	impl1(ctx)

	impl2(ctx)
}
