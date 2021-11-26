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
    with tempfile.NamedTemporaryFile(dir = TMP_UPLOAD_DIR, delete = False) as fp:
        fp.write(os.urandom(90000000))
        yield fp

# DELETE
def test_delete_200(tmp_webserv_dir, tmp_file):
    # implement
    ## url =  'http://localhost:8084/' + tmp_file.name
    ## response = requests.delete(url)
    ## assert response.status_code == 200
    assert os.path.isfile(tmp_file.name) == True
    os.remove(tmp_file.name)
    assert os.path.isfile(tmp_file.name) == False
