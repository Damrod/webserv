import os
from pathlib import Path
import psutil
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
    url = 'http://localhost:8094/info.php'
    response = requests.get(url)
    assert response.status_code == 500

def test_cgi_redirect_301():
    url = 'http://localhost:8094/redirect.py'
    redirect_url = "https://www.google.com"
    response = requests.get(url, allow_redirects=False)
    assert response.status_code == 301
    assert response.headers['Location'] == redirect_url

def test_invalid_no_output_cgi_script_500():
    url = 'http://localhost:8094/no_output.py'
    response = requests.get(url)
    assert response.status_code == 500

def test_invalid_headers_cgi_script_500():
    url = 'http://localhost:8094/invalid_headers.py'
    response = requests.get(url, timeout=5)
    process_is_running = False
    for process in psutil.process_iter():
        arg = 'html/web6/invalid_headers.py'
        try:
            if arg in process.cmdline():
                process_is_running = True
        except:
            pass
    assert not process_is_running
    assert response.status_code == 500
