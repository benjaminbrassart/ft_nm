FROM alpine:3.20 AS build

ARG MELKOR_REPOSITORY=https://github.com/IOActive/Melkor_ELF_Fuzzer.git
ARG MELKOR_COMMIT=3ef15ca

RUN apk -U upgrade && apk add --no-cache build-base git

WORKDIR /build

RUN git clone --depth=1 -- ${MELKOR_REPOSITORY} melkor

WORKDIR /build/melkor

RUN git checkout ${MELKOR_COMMIT}

COPY melkor.patch .
RUN git apply --reject --whitespace=fix melkor.patch

RUN make CFLAGS="-fdiagnostics-color -Wall -Wextra"

FROM alpine:3.20

COPY --from=build /build/melkor/melkor /usr/local/bin/melkor
COPY --from=build /build/melkor/test_fuzzed.sh /usr/local/bin/test_fuzzed
COPY --from=build /build/melkor/templates /melkor/templates

VOLUME /melkor/orcs

WORKDIR /melkor/orcs
