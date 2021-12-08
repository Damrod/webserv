from pathlib import Path
import pytest
import requests
import subprocess
import time

PROJ_DIR = str(Path(__file__).parents[4])

@pytest.fixture(scope='module', autouse=True)
def start_webserv():
    time.sleep(1)
    webserv = subprocess.Popen('./webserv', cwd=PROJ_DIR)
    time.sleep(2)
    yield webserv
    webserv.terminate()

def test_same_port_different_server_name():
    headers = {'Host': 'example.com'}
    response = requests.get('http://localhost:8085/', headers=headers)
    assert response.status_code == 200
    assert response.content.decode('UTF-8') == 'web4\n'
    headers = {'Host': 'example2.com'}
    response = requests.get('http://localhost:8085/', headers=headers)
    assert response.status_code == 200
    assert response.content.decode('UTF-8') == 'web5\n'

def test_same_port_same_server_name():
    headers = {'Host': 'example.com'}
    response = requests.get('http://localhost:8086/', headers=headers)
    assert response.status_code == 200
    assert response.content.decode('UTF-8') == 'web4\n'

def test_different_port_same_server_name():
    headers = {'Host': 'example.com'}
    response = requests.get('http://localhost:8087/', headers=headers)
    assert response.status_code == 200
    assert response.content.decode('UTF-8') == 'web4\n'
    headers = {'Host': 'example.com'}
    response = requests.get('http://localhost:8088/', headers=headers)
    assert response.status_code == 200
    assert response.content.decode('UTF-8') == 'web5\n'

def test_no_server_name():
    headers = {'Host': 'example.com'}
    response = requests.get('http://localhost:8089/', headers=headers)
    assert response.status_code == 200
    assert response.content.decode('UTF-8') == 'web4\n'

def test_no_server_name_same_port():
    headers = {'Host': 'example.com'}
    response = requests.get('http://localhost:8090/', headers=headers)
    assert response.status_code == 200
    assert response.content.decode('UTF-8') == 'web4\n'
    response = requests.get('http://localhost:8090/', headers=headers)
    assert response.status_code == 200
    assert response.content.decode('UTF-8') == 'web4\n'

def test_no_server_name_different_port():
    headers = {'Host': 'example.com'}
    response = requests.get('http://localhost:8091/', headers=headers)
    assert response.status_code == 200
    assert response.content.decode('UTF-8') == 'web4\n'
    response = requests.get('http://localhost:8092/', headers=headers)
    assert response.status_code == 200
    assert response.content.decode('UTF-8') == 'web5\n'

def test_wildcard_server_name():
    headers = {'Host': 'example.com'}
    response = requests.get('http://localhost:8093/', headers=headers)
    assert response.status_code == 200
    assert response.content.decode('UTF-8') == 'web4\n'
    headers = {'Host': 'example1.com'}
    response = requests.get('http://localhost:8093/', headers=headers)
    assert response.status_code == 200
    assert response.content.decode('UTF-8') == 'web5\n'

def test_default_server_name():
    response = requests.get('http://localhost:8093/')
    assert response.status_code == 200
    assert response.content.decode('UTF-8') == 'web4\n'
