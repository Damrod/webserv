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

def test_invalid_cgi_binary_500():
    url = 'http://localhost:8085/info.php'
    response = requests.get(url)
    assert response.status_code == 500
