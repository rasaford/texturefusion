# This scrips collects the current GPU usage and writes it to a file
# Adaped from: https://superuser.com/questions/1632758/how-to-get-gpu-usage-and-gpu-memory-info-of-a-process-by-powershell7
# outputs a JSON file

$out_file = $args[0]
$process_name = "DepthSensing"
"[" | Out-File -FilePath $out_file
$latch = $true
while ($latch -or $process) {
    $process = Get-Process $process_name -ErrorAction SilentlyContinue
    if ($process) {
        $latch = $false
        # Get gpu usage
        $GpuMemTotal = ((Get-Counter "\GPU Process Memory(pid_$($process.id)*)\Local Usage").CounterSamples | where CookedValue).CookedValue
        $GpuUseTotal = ((Get-Counter "\GPU Engine(pid_$($process.id)*engtype_3D)\Utilization Percentage").CounterSamples | where CookedValue).CookedValue 
        
        $mem_total_out = $GpuMemTotal -join ", "
        $use_total_out = $GpuUseTotal -join ", "
        $time = Get-Date -Format HH:mm:ss.fff
        $out = "{ 'time': '$time', 'gpu_mem_total': [$mem_total_out], 'gpu_use_total': [$use_total_out] },"
        Add-Content $out_file $out 
        Write-Output $out
    }
}
Add-Content $out_file "]"
# replace single quote strings with double quote ones
((Get-Content -path $out_file -Raw) -replace "'",'"') | Set-Content -Path $out_file