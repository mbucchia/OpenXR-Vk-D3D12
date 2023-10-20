@echo off
pushd %~dp0
wpr -start OpenXR-Vk-D3D12.wprp -filemode

echo Reproduce your issue now, then
pause

wpr -stop Trace.etl
popd
