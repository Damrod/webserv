import os
from pathlib import Path
import pytest
import requests
import subprocess
import tempfile
import time

PROJ_DIR = str(Path(__file__).parents[4])
TMP_UPLOAD_DIR = PROJ_DIR + '/html/web3/upload/'

@pytest.fixture(scope='module', autouse=True)
def start_webserv():
    webserv = subprocess.Popen('./webserv', cwd=PROJ_DIR)
    time.sleep(2)
    yield webserv
    webserv.terminate()

@pytest.fixture(scope='module')
def tmp_file():
    with tempfile.NamedTemporaryFile(dir = TMP_UPLOAD_DIR, delete = False) as fp:
        fp.write(os.urandom(90000000))
        yield fp

@pytest.fixture(scope='module')
def tmp_forbidden_file():
    with tempfile.NamedTemporaryFile(dir = TMP_UPLOAD_DIR) as fp:
        fp.write(os.urandom(90000000))
        yield fp

# SUCCESSFUL RESPONSES

def test_delete_file_200(tmp_file):
    assert os.path.isfile(tmp_file.name) == True
    url =  'http://localhost:8084/upload/' + tmp_file.name
    response = requests.delete(url)
    assert response.status_code == 200
    # os.remove(tmp_file.name)
    assert os.path.isfile(tmp_file.name) == False

def test_delete_slash_ended_path_200():
    url =  'http://localhost:8084/upload/'
    response = requests.delete(url)
    assert response.status_code == 200
    # comprobar borrado carpeta


# ERROR RESPONSES

def test_delete_path_409(tmp_file):
    url =  'http://localhost:8084/upload'
    response = requests.delete(url)
    assert response.status_code == 409

def test_delete_forbidden_resource_403(tmp_forbidden_file):
    url =  'http://localhost:8084/upload/' + tmp_forbidden_file
    response = requests.delete(url)
    assert response.status_code == 403

def test_non_existent_file_404():
    url =  'http://localhost:8084/upload/non_existent.txt'
    response = requests.delete(url)
    assert response.status_code == 404

@pytest.mark.skip(reason="building")
def test_forward_traversal_delete_403():
    url =  'http://localhost:8084/../upload'

@pytest.mark.skip(reason="building")
def test_backwards_traversal_delete_403():
    url =  'http://localhost:8084/upload/new_folder'
