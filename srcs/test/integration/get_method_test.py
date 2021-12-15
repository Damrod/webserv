import aiohttp
import asyncio
import os
import requests
import time

def test_get_autoindex_200():
    url = 'http://localhost:8080'
    response = requests.get(url)
    assert response.status_code == 200

def test_get_200():
    url = 'http://localhost:8081'
    response = requests.get(url)
    assert response.status_code == 200

def test_get_redirect_301():
    url = 'http://localhost:8083'
    response = requests.get(url, allow_redirects=False)
    redirect_url = "https://www.google.com"
    assert response.status_code == 301
    assert response.headers['Location'] == redirect_url

def test_get_follow_redirect_200():
    url = 'http://localhost:8083'
    response = requests.get(url)
    assert response.status_code == 200

def test_get_404():
    url = 'http://localhost:8080/..invalidpath'
    response = requests.get(url)
    assert response.status_code == 404

def test_get_cgi_200():
    params = {'fname': 'first_name', 'lname': 'last_name'}
    url = 'http://localhost:8084/cgi-bin/hello_form.py'
    response = requests.get(url, params=params)
    assert response.status_code == 200
    assert 'first_name' in response.text
    assert 'last_name' in response.text

def test_get_http_redirect_location_301():
    url = 'http://localhost:8081/google'
    response = requests.get(url, allow_redirects=False)
    redirect_url = "https://www.google.com"
    assert response.status_code == 301
    assert response.headers['Location'] == redirect_url

def test_get_index_cgi_200():
    url = 'http://localhost:8084/hello/'
    response = requests.get(url)
    assert response.status_code == 200

def test_get_index_cgi_query_string_200():
    url = 'http://localhost:8084/hello_form/?fname=hello&lname=world'
    response = requests.get(url)
    assert response.status_code == 200
    assert "hello" in response.text
    assert "world" in response.text

async def async_get_request(session, url):
    async with session.get(url) as resp:
        return resp.status

async def async_get_slow_cgi(requests_number, seconds):
    async with aiohttp.ClientSession() as session:
        tasks = []
        url = 'http://localhost:8084/cgi-bin/slow_cgi.py?seconds=' + str(seconds)
        for _ in range(requests_number):
            tasks.append(asyncio.ensure_future(async_get_request(session, url)))

        statuses = await asyncio.gather(*tasks)
        for status in statuses:
            assert status == 200

def test_get_env_slow_cgi():
    requests_number = 20
    seconds = 2
    start_time = time.time()
    asyncio.run(async_get_slow_cgi(requests_number, seconds))
    end_time = time.time()
    assert end_time - start_time < requests_number * seconds

def test_url_path_traversal():
    headers = {'Host': 'localhost:8080'}
    url = 'http://localhost:8080/../config/default.conf'
    session = requests.Session()
    request = requests.Request(method='GET', url=url, headers=headers)
    prep = request.prepare()
    prep.url = url
    response = session.send(prep)
    assert response.status_code == 400

def test_custom_error_page_404():
    url = 'http://localhost:8082/invalidpath'
    response = requests.get(url)
    assert response.status_code == 404
    assert "main.css" in response.text
