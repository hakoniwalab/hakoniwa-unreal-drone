# Hakoniwa Unreal Drone

このリポジトリは、箱庭ドローンシミュレータのビジュアライズ機能を提供する Unreal Engine プロジェクトです。
Unreal Engine 5.6 を使用し、WebSocket を介して取得した PDU 情報からドローンの状態を描画します。


## セットアップ

1. 本リポジトリをクローンします。サブモジュールを利用しているため `--recursive` オプションを付けてください。
   ```bash
   git clone --recursive <repository_url>
   ```
   既にクローン済みの場合は、次のコマンドでサブモジュールを初期化します。
   ```bash
   git submodule update --init --recursive
   ```
2. Unreal Engine 5.6 以降をインストールします。
3. `HakoniwaDrone.uproject` を Unreal Editor で開きます。
4. 必要に応じてビルドを実行し、プロジェクトを起動します。

## 利用可能な Level

- `AvatarWeb.umap`  — ドローンを Web 経由で制御する際に使用するメインのレベルです。

## 主要な Blueprint

- `BP_HakoniwaAvatar` — ドローン本体を表すブループリント。
- `BP_HakoniwaWebClient` — WebSocket 通信と PDU 管理を行うブループリント。

それぞれ `Content/Blueprints` フォルダ内に配置されています。

## コンフィグレーション

`Content/Config/webavatar.json` に PDU 設定が記述されています。Web クライアントはこのファイルを読み込み、各種メッセージの受信設定を行います。

## コードの概要

本プロジェクトでは `AHakoniwaWebClient` が WebSocket 経由で PDU を受信し、`AHakoniwaAvatar` にドローンの位置と回転を反映させます。`UDronePropellerComponent` は各プロペラを回転させ、モーターの回転数を視覚化します。

PDU 取得や通信処理には [hakoniwa-pdu-unreal](https://github.com/hakoniwalab/hakoniwa-pdu-unreal) プラグインを使用しています。プラグインは `UPduManager` と `UWebSocketCommunicationService` を提供し、これらを通じて PDU の読み書きや接続管理を行います。

`Plugins/HakoniwaPdu` フォルダはサブモジュールになっているため、クローン後に `git submodule update --init --recursive` を実行して取得してください。

## ライセンス

本リポジトリは MIT License の下で公開されています。詳細は `LICENSE` ファイルを参照してください。
