FROM alpine:3.12.3
# Add user, install packages
RUN	apk add --update --no-cache --allow-untrusted nginx \
	&& adduser -D -g 'www' www \
	&& chown -R www:www /var/lib/nginx \
	&& mkdir -p /run/nginx /var/www/html \
	&& chown -R www:www /var/www
COPY srcs/index.html /var/www/html/
COPY srcs/docker-entrypoint.sh /
RUN chown -R www:www /var/www/html/* \
	&& rm /etc/nginx/nginx.conf
EXPOSE 80 8080
ENTRYPOINT ["nginx","-t"]

# docker run -v $(pwd)/vol/nginx.conf:/etc/nginx/nginx.conf -v $(pwd)/vol/http.d/default.conf:/etc/nginx/http.d/default.conf -p 8080:80 -it mynginx:2.0 /bin/sh 
