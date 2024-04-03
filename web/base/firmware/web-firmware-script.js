'use strict';

var file;
var xhr;

function logUpload(text)
{
    document.getElementById('uploadresult').textContent = text;
}
function xhrUploadResponse()
{
    document.getElementById('restartbutton').disabled = false;
    var topics = xhr.responseText.split('\f');
    logUpload(topics[0]);
    if (topics.length > 1) document.getElementById('list').textContent = topics[1];
}
function xhrUploadOnLoad()
{
    if (xhr.status == 200) xhrUploadResponse();
    else                   logUpload('Upload failed');
}
function xhrUploadOnError()
{
    logUpload('Upload error');
}
function xhrUploadProgress(e)
{
    logUpload('Uploading ' + e.total + ' bytes ' + '(' + Math.round(e.loaded/e.total * 100) + '%)...\r\n');
}
function xhrUploadComplete(e)
{
    logUpload('Saving ' + e.loaded  + ' bytes...');
}
function xhrUploadStart()
{
    document.getElementById('restartbutton').disabled = true;
    logUpload('Upload starting...');
    
    xhr = new XMLHttpRequest();

    xhr.onload  = xhrUploadOnLoad;
    xhr.onerror = xhrUploadOnError;
    xhr.upload.onprogress = xhrUploadProgress;
    xhr.upload.onload     = xhrUploadComplete;

    xhr.open('POST', '/firmware-ajax'); //Defaults to async=true
    xhr.send(file);
}
function startUpload()
{
    var fileInput = document.getElementById('fileInput');

    if (fileInput.files.length == 0)
    {
        logUpload('Please choose a file');
        return;
    }

    if (fileInput.files.length > 1)
    {
        logUpload('Please choose just one file');
        return;
    }
    
    file = fileInput.files[0];
    
    xhrUploadStart();
}
function logRestart(text)
{
    document.getElementById('restartresult').textContent = text;
}
function redirect()
{
    location.href = '/firmware';
}
function xhrRestartOnLoad()
{
    if (xhr.status == 200) logRestart('Restart should never have returned');
    else                   logRestart('Restart failed');
}
function xhrRestartStart()
{
    logRestart('Restarting...');
    
    xhr = new XMLHttpRequest();

    xhr.onload  = xhrRestartOnLoad;

    xhr.open('GET', '/firmware-ajax?restart='); //Defaults to async=true
    xhr.send();
    
    setTimeout(redirect, 2000);
}
function restart()
{
    xhrRestartStart();
}

