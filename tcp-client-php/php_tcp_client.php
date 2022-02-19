<?php
$fp = fsockopen("192.168.0.123", 5188, $errno, $errstr, 30);


if (!$fp) {
    echo "$errstr ($errno)<br />\n";
} else {

    sleep(3);
    fwrite($fp, "data-1");
    sleep(30);
    fwrite($fp, "data-2");
    printf("发送 data-2 时间 %s \r\n",time());

    while (!feof($fp)) {
        echo fgets($fp, 128);
    }
    printf("收到链接断开 时间 %s \r\n",time());
    echo("end \r\n");

    fclose($fp);
}
?>