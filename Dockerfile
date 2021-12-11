FROM debian:bullseye-slim
COPY . /webserv/
RUN apt-get update \
 && apt-get install -y \
 build-essential \
 coreutils \
 wget \
 bc \
 python2.7 \
 php7.4-fpm \
 php7.4-common \
 php7.4-mbstring \
 php7.4-xmlrpc \
 php7.4-soap \
 php7.4-intl \
 php7.4-mysql \
 php7.4-cli \
 php7.4-ldap \
 php7.4-zip \
 php7.4-curl \
 php7.4-json \
 php7.4-opcache \
 php7.4-readline \
 php7.4-xml \
 php7.4-gd \
 php7.4-cgi \
 && make CXX=g++ -j12 -C /webserv/ \
 && wget -q https://wordpress.org/latest.tar.gz -P /tmp/ \
 && tar xzf /tmp/latest.tar.gz -C /tmp/ \
 && cp -r /tmp/wordpress/* /webserv/html/wordpress/ \
 && cd /webserv/html/wordpress/ \
 && wget -q https://api.wordpress.org/secret-key/1.1/salt/ -O salt \
 && csplit -s wp-config-sample.php '/AUTH_KEY/' '/NONCE_SALT/+1' \
 && cat xx00 salt xx02 > wp-config.php \
 && rm salt xx00 xx01 xx02 \
 && rm -rf /tmp/*
EXPOSE 8080
EXPOSE 8081
EXPOSE 8082
EXPOSE 8083
EXPOSE 8084
EXPOSE 8085
EXPOSE 8086
EXPOSE 8087
EXPOSE 8088
EXPOSE 8089
EXPOSE 8090
EXPOSE 8091
EXPOSE 8092
EXPOSE 8093
EXPOSE 8094
EXPOSE 9000
RUN chmod +x /webserv/docker-entrypoint.sh
ENTRYPOINT ["./webserv/docker-entrypoint.sh"]
