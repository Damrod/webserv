import aiohttp
import asyncio
import filecmp
import os
from pathlib import Path
import pytest
import requests
import shutil
import subprocess
import tempfile
import time
import uuid

TMP_WEBSERV_DIR = '/tmp/webserv/'
PROJ_DIR = str(Path(__file__).parents[4])
TMP_UPLOAD_DIR = PROJ_DIR + '/html/web3/test/'

@pytest.fixture(scope='module', autouse=True)
def start_webserv():
    webserv = subprocess.Popen('./webserv', cwd=PROJ_DIR)
    time.sleep(2)
    yield webserv
    webserv.terminate()

@pytest.fixture(scope='module')
def tmp_webserv_dir():
    if not os.path.exists(TMP_WEBSERV_DIR):
        os.mkdir(TMP_WEBSERV_DIR)
    yield
    shutil.rmtree(TMP_WEBSERV_DIR)

@pytest.fixture(scope='module')
def tmp_file():
    with tempfile.NamedTemporaryFile() as fp:
        fp.write(os.urandom(90000000))
        yield fp

@pytest.fixture(scope='function')
def random_filename():
    return str(uuid.uuid4())

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

def test_method_not_implemented_501():
    url = 'http://localhost:8080/'
    response = requests.options(url)
    assert response.status_code == 501

def test_post_cgi_200():
    payload = {'fname': 'first_name', 'lname': 'last_name'}
    url = 'http://localhost:8084/cgi-bin/hello_form.py'
    response = requests.post(url, data=payload)
    assert response.status_code == 200
    assert 'first_name' in response.text
    assert 'last_name' in response.text

def test_get_cgi_200():
    params = {'fname': 'first_name', 'lname': 'last_name'}
    url = 'http://localhost:8084/cgi-bin/hello_form.py'
    response = requests.get(url, params=params)
    assert response.status_code == 200
    assert 'first_name' in response.text
    assert 'last_name' in response.text

def test_post_upload_cgi_200(tmp_webserv_dir, tmp_file):
    url =  'http://localhost:8084/cgi-bin/save_file.py'
    filename = 'file.test'
    files = {'filename': (filename, open(tmp_file.name, 'rb'))}
    response = requests.post(url, files=files)
    assert response.status_code == 200
    filepath = TMP_WEBSERV_DIR + filename
    assert filecmp.cmp(filepath, tmp_file.name)

def test_post_too_large_413(tmp_webserv_dir, tmp_file):
    url =  'http://localhost:8082'
    filename = 'file.test'
    files = {'filename': (filename, open(tmp_file.name, 'rb'))}
    response = requests.post(url, files=files)
    assert response.status_code == 413

def test_post_upload_200(tmp_file):
    url =  'http://localhost:8084/upload/'
    filename = 'file.test'
    mime_type = 'application/octet-stream'
    files = {'filename': (filename, open(tmp_file.name, 'rb'), mime_type)}
    response = requests.post(url, files=files)
    assert response.status_code == 200
    filepath = TMP_UPLOAD_DIR + filename
    assert filecmp.cmp(filepath, tmp_file.name)
    os.remove(filepath)

def test_url_path_traversal():
    headers = {'Host': 'localhost:8080'}
    url = 'http://localhost:8080/../config/default.conf'
    session = requests.Session()
    request = requests.Request(method='GET', url=url, headers=headers)
    prep = request.prepare()
    prep.url = url
    response = session.send(prep)
    assert response.status_code == 400

def test_file_upload_path_traversal(tmp_webserv_dir, random_filename):
    url =  'http://localhost:8084/upload/'
    filename = '../../../../../../../../../../../../../../tmp/webserv/' + random_filename
    mime_type = 'text/plain'
    files = {'file': (filename, 'random text\n', mime_type)}
    response = requests.post(url, files=files)
    assert not os.path.exists('/tmp/webserv/' + random_filename)
    assert response.status_code == 400

def test_http_redirect_location_301():
    url = 'http://localhost:8081/google'
    response = requests.get(url, allow_redirects=False)
    redirect_url = "https://www.google.com"
    assert response.status_code == 301
    assert response.headers['Location'] == redirect_url

def test_index_cgi_200():
    url = 'http://localhost:8084/hello/'
    response = requests.get(url)
    assert response.status_code == 200

def test_index_cgi_query_string_200():
    url = 'http://localhost:8084/hello_form/?fname=hello&lname=world'
    response = requests.get(url)
    assert response.status_code == 200
    assert "hello" in response.text
    assert "world" in response.text

def test_percent_encoding():
    upload_url =  'http://localhost:8084/upload/'
    filename = 'filename containing space.txt'
    mime_type = 'text/plain'
    files = {'file': (filename, 'random text\n', mime_type)}
    upload_response = requests.post(upload_url, files=files)
    filepath = TMP_UPLOAD_DIR + filename
    assert os.path.exists(TMP_UPLOAD_DIR + filename)
    url = 'http://localhost:8084/test/filename+containing%20space.txt'
    response = requests.get(url)
    os.remove(filepath)
    assert upload_response.status_code == 200
    assert response.status_code == 200

def test_post_upload_non_existing_path(random_filename):
    url =  'http://localhost:8084/send'
    mime_type = 'text/plain'
    files = {'file': (random_filename, 'random text\n', mime_type)}
    response = requests.post(url, files=files)
    filepath = TMP_UPLOAD_DIR + random_filename
    assert os.path.exists(filepath)
    os.remove(filepath)
    assert response.status_code == 200

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
