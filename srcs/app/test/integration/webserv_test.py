import filecmp
import os
from pathlib import Path
import pytest
import requests
import shutil
import subprocess
import tempfile
import time

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
    url = 'http://localhost:8080/invalidpath'
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
