#!/usr/bin/env sh

sed -i -e "s/database_name_here/${WORDPRESS_DB_NAME}/" \
	-e "s/username_here/${WORDPRESS_DB_USER}/" \
	-e "s/password_here/${WORDPRESS_DB_PASSWORD}/" \
	-e "s/localhost/mysql/" /webserv/html/wordpress/wp-config.php

cd /webserv/
exec ./webserv
