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
def tmp_dir():
    yield tempfile.mkdtemp(dir = TMP_UPLOAD_DIR)

@pytest.fixture(scope='module')
def tmp_file_inside_dir(tmp_dir):
    with tempfile.NamedTemporaryFile(dir = tmp_dir, delete = False) as fp:
        fp.write(os.urandom(90000000))
        yield fp

@pytest.fixture(scope='module')
def tmp_forbidden_file():
    with tempfile.NamedTemporaryFile(dir = TMP_UPLOAD_DIR) as fp:
        fp.write(os.urandom(90000000))
        os.chflags(fp.name, os.UF_NOUNLINK)
        yield fp

# SUCCESSFUL RESPONSES

def test_delete_regular_file_200(tmp_file):
    assert os.path.isfile(tmp_file.name) == True
    single_file_name = tmp_file.name.split('/')[-1]
    url =  'http://localhost:8084/upload/' + single_file_name
    response = requests.delete(url)
    assert response.status_code == 200
    assert os.path.isfile(tmp_file.name) == False

def test_delete_slash_ended_dir_200(tmp_dir):
    single_dir_name = tmp_dir.split('/')[-1]
    assert os.path.isdir(tmp_dir) == True
    url =  'http://localhost:8084/upload/' + single_dir_name + '/'
    response = requests.delete(url)
    assert response.status_code == 200
    assert os.path.isdir(tmp_dir) == False

def test_delete_dir_and_file_inside_dir_200(tmp_dir, tmp_file_inside_dir):
    single_dir_name = tmp_dir.split('/')[-1]
    assert os.path.isdir(tmp_dir) == True
    assert os.path.isfile(tmp_file_inside_dir.name) == True
    print(tmp_dir)
    print(tmp_file_inside_dir.name)
    url =  'http://localhost:8084/upload/' + single_dir_name + '/'
    response = requests.delete(url)
    assert response.status_code == 200
    assert os.path.isdir(tmp_dir) == False
    assert os.path.isfile(tmp_file_inside_dir.name) == False

# ERROR RESPONSES

def test_delete_non_slash_ended_dir_409(tmp_dir):
    single_dir_name = tmp_dir.split('/')[-1]
    assert os.path.isdir(tmp_dir) == True
    url =  'http://localhost:8084/upload/' + single_dir_name
    response = requests.delete(url)
    assert response.status_code == 409
    assert os.path.isdir(tmp_dir) == True

def test_delete_forbidden_resource_403(tmp_forbidden_file):
    print(tmp_forbidden_file.name)
    assert os.path.isfile(tmp_forbidden_file.name) == True
    single_file_name = tmp_forbidden_file.name.split('/')[-1]
    url =  'http://localhost:8084/upload/' + single_file_name
    response = requests.delete(url)
    assert response.status_code == 403
    assert os.path.isfile(tmp_forbidden_file.name) == True

def test_delete_non_existent_file_404():
    url =  'http://localhost:8084/upload/non_existent.txt'
    response = requests.delete(url)
    assert response.status_code == 404

@pytest.mark.skip(reason="building")
def test_path_traversal_delete_403():
    url =  'http://localhost:8084/../upload/'
    response = requests.delete(url)
    assert response.status_code == 403
