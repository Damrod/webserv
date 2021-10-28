import pytest
import requests
import subprocess
import time

@pytest.fixture(scope='session')
def start_webserv():
    webserv = subprocess.Popen(['./webserv'])
    time.sleep(2)
    yield webserv
    webserv.terminate()

def test_get_200_autoindex(start_webserv):
    response = requests.get('http://localhost:8080')
    assert response.status_code == 200

def test_get_200(start_webserv):
    response = requests.get('http://localhost:8081')
    assert response.status_code == 200

def test_get_301(start_webserv):
    response = requests.get('http://localhost:8083', allow_redirects=False)
    assert response.status_code == 301
    assert response.headers['Location'] == "https://www.google.com"

def test_get_200_after_redirect(start_webserv):
    response = requests.get('http://localhost:8083')
    assert response.status_code == 200

def test_get_404(start_webserv):
    response = requests.get('http://localhost:8080/invalidpath')
    assert response.status_code == 404

def test_options_501(start_webserv):
    response = requests.options('http://localhost:8080/invalidpath')
    assert response.status_code == 501

def test_post_200(start_webserv):
    payload = {'fname': 'first_name', 'lname': 'last_name'}
    response = requests.post('http://localhost:8084/cgi-bin/hello_form.py',
                                data=payload)
    assert response.status_code == 200
