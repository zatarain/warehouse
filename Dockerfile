FROM zatarain/cpp:development
ENV LD_LIBRARY_PATH=/usr/local/lib \
	TERM=xterm \
	PATH=${PATH}:/build/bin
RUN echo "${TERM}" && tabs -4
WORKDIR /root
RUN apt-get update && apt-get -y install jq && rm -rf /var/lib/apt/lists/*
RUN git clone --depth 1 --branch v1.1.0 https://github.com/Tencent/rapidjson.git
RUN git clone https://github.com/zatarain/utz.git
COPY . .
WORKDIR /root/utz
RUN make install && rm -rf aut
WORKDIR /root
RUN utz test
RUN make out/warehouse
ENTRYPOINT ["out/warehouse"]