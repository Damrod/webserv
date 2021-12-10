import os
from pathlib import Path
import pytest
import shutil
import subprocess
import tempfile
import time
import uuid

PROJ_DIR = str(Path(__file__).parents[3])
TMP_UPLOAD_DIR = PROJ_DIR + '/html/web3/upload/'
TMP_TEST_DIR = PROJ_DIR + '/html/web3/test/'
TMP_WEBSERV_DIR = '/tmp/webserv/'

@pytest.fixture(scope='session', autouse=True)
def start_webserv():
    webserv = subprocess.Popen('./webserv', cwd=PROJ_DIR)
    time.sleep(2)
    yield webserv
    webserv.terminate()

@pytest.fixture(scope='function')
def random_filename():
    return str(uuid.uuid4())

@pytest.fixture(scope='session')
def tmp_webserv_dir():
    if not os.path.exists(TMP_WEBSERV_DIR):
        os.mkdir(TMP_WEBSERV_DIR)
    yield
    shutil.rmtree(TMP_WEBSERV_DIR)

@pytest.fixture(scope='session')
def tmp_file():
    with tempfile.NamedTemporaryFile() as fp:
        fp.write(os.urandom(90000000))
        yield fp

@pytest.fixture(scope='function')
def random_file(random_filename):
    filepath = TMP_TEST_DIR + random_filename
    open(filepath, 'a').close()
    yield filepath
    if os.path.exists(filepath):
        os.remove(filepath)

@pytest.fixture(scope='function')
def random_dir(random_filename):
    dirpath = TMP_TEST_DIR + random_filename
    os.makedirs(dirpath, exist_ok=True)
    yield dirpath
    if os.path.exists(dirpath):
        os.rmdir(dirpath)

@pytest.fixture(scope='function')
def random_dir_with_file(random_filename):
    dirpath = TMP_TEST_DIR + random_filename
    filepath = dirpath + '/' + random_filename
    os.makedirs(dirpath, exist_ok=True)
    open(filepath, 'a').close()
    yield dirpath
    if os.path.exists(dirpath):
        shutil.rmtree(dirpath)
