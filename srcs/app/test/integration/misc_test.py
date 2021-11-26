import os
from pathlib import Path
import pytest
import requests
import shutil
import subprocess
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

@pytest.fixture(scope='function')
def random_filename():
    return str(uuid.uuid4())

# MISC
def test_method_not_implemented_501():
    url = 'http://localhost:8080/'
    response = requests.options(url)
    assert response.status_code == 501

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
