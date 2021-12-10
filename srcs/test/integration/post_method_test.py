from conftest import TMP_TEST_DIR
from conftest import TMP_WEBSERV_DIR
import filecmp
import os
import requests

# POST
def test_post_cgi_200():
    payload = {'fname': 'first_name', 'lname': 'last_name'}
    url = 'http://localhost:8084/cgi-bin/hello_form.py'
    response = requests.post(url, data=payload)
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
    filepath = TMP_TEST_DIR + filename
    assert filecmp.cmp(filepath, tmp_file.name)
    os.remove(filepath)

def test_percent_encoding():
    upload_url =  'http://localhost:8084/upload/'
    filename = 'filename containing space.txt'
    mime_type = 'text/plain'
    files = {'file': (filename, 'random text\n', mime_type)}
    upload_response = requests.post(upload_url, files=files)
    filepath = TMP_TEST_DIR + filename
    assert os.path.exists(TMP_TEST_DIR + filename)
    url = 'http://localhost:8084/test/filename+containing%20space.txt'
    response = requests.get(url)
    os.remove(filepath)
    assert upload_response.status_code == 200
    assert response.status_code == 200

def test_file_upload_path_traversal(tmp_webserv_dir, random_filename):
    url =  'http://localhost:8084/upload/'
    filename = '../../../../../../../../../../../../../../tmp/webserv/' + random_filename
    mime_type = 'text/plain'
    files = {'file': (filename, 'random text\n', mime_type)}
    response = requests.post(url, files=files)
    assert not os.path.exists('/tmp/webserv/' + random_filename)
    assert response.status_code == 400
    
def test_post_upload_non_existing_path(random_filename):
    url =  'http://localhost:8084/send'
    mime_type = 'text/plain'
    files = {'file': (random_filename, 'random text\n', mime_type)}
    response = requests.post(url, files=files)
    filepath = TMP_TEST_DIR + random_filename
    assert os.path.exists(filepath)
    os.remove(filepath)
    assert response.status_code == 200
