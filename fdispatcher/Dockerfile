FROM alpine:3.7 as build

RUN apk add --no-cache alpine-sdk make

COPY . /tmp/work

WORKDIR /tmp/work

RUN make

FROM alpine:3.7 as run

COPY --from=build /tmp/work/build/* /usr/local/bin

#HEALTHCHECK 

CMD fdispatcher
