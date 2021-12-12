import os
import pytest
import requests

# SUCCESSFUL RESPONSES

def test_delete_regular_file_200(random_file):
    single_file_name = random_file.split('/')[-1]
    url =  'http://localhost:8084/test/' + single_file_name
    response = requests.delete(url)
    assert response.status_code == 200
    assert os.path.exists(random_file) == False

def test_delete_slash_ended_dir_200(random_dir):
    single_dir_name = random_dir.split('/')[-1]
    url =  'http://localhost:8084/test/' + single_dir_name + '/'
    response = requests.delete(url)
    assert response.status_code == 200
    assert os.path.exists(random_dir) == False

def test_delete_dir_and_file_inside_dir_200(random_dir_with_file):
    single_dir_name = random_dir_with_file.split('/')[-1]
    url =  'http://localhost:8084/test/' + single_dir_name + '/'
    response = requests.delete(url)
    assert response.status_code == 200
    assert os.path.exists(random_dir_with_file) == False

# ERROR RESPONSES

def test_delete_non_slash_ended_dir_409(random_dir):
    single_dir_name = random_dir.split('/')[-1]
    url =  'http://localhost:8084/test/' + single_dir_name
    response = requests.delete(url)
    assert response.status_code == 409
    assert os.path.exists(random_dir) == True

# This test may be problematic when running locally
# since it needs no_priviliges_folder and its content
# needs to be owned by a different user an a group than current.
# If encountering problems, just disable it
@pytest.mark.skip(reason="doesn't work on campus")
def test_delete_forbidden_resource_403():
    url =  'http://localhost:8084/no_privileges_folder/'
    response = requests.delete(url)
    assert response.status_code == 403

def test_delete_not_allowed_path_405():
    url =  'http://localhost:8084/images/'
    response = requests.delete(url)
    assert response.status_code == 405

def test_delete_non_existent_file_404():
    url =  'http://localhost:8084/test/non_existent.txt'
    response = requests.delete(url)
    assert response.status_code == 404
