# Theta S用のEquirectangular変換


## 使い方

`$ ec_thetas <input file> <output file>`

<input file>にTheta Sで録画したDual Fisheye形式の動画ファイル、<output file>に変換後のファイル名を指定して実行すると、
Equirectangular形式に変換されて出力される。音声には対応していない。


### 実行例
```
$ ec_thetas in720p.mp4 out720p.mp4
Input file  : in720p.mp4
Output file : out720p.mp4
size = 1280x720
frame count = 240
fps = 29.97
........
```

## 解説

- リアルタイムで処理ができるように、初めにEquirectangular → Dual Fisheyeの写像行列を作成しておいて、OpenCVのremap()関数を使用することにより高速に変換を行うようにした。
- Kurento Media Serverのフィルタとして取り込んで動作させるため、OpenCV 2.4を使用している。
