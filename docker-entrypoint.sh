#!/usr/bin/env bash

sed -i -e "s/database_name_here/${WORDPRESS_DB_NAME}/" \
	-e "s/username_here/${WORDPRESS_DB_USER}/" \
	-e "s/password_here/${WORDPRESS_DB_PASSWORD}/" \
	-e "s/localhost/${WORDPRESS_DB_HOST}/" /webserv/html/wordpress/wp-config.php

cat << EOF >> /webserv/config/default.conf

server {
	listen			9000;
	root			html/wordpress;
	index			index.php;
	cgi_assign .php	/usr/bin/php-cgi;
}
EOF

cd /webserv/
exec ./webserv
