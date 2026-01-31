# include <Siv3D.hpp>

// ガチャを管理するクラス
class Gacha {

	public:
		// 最初9回の確率
		const Array<double> probabilities = { 0.5, 0.3, 0.15, 0.04, 0.01 }; // 例: ノーマル50%, レア30%, スーパーレア15%, ウルトラレア4%, レジェンド1%
		// 最後の1回の確率 (スーパーレア以上確定)
		const Array<double> lastProbabilities = { 0.0, 0.0, 0.60, 0.35, 0.05 };
		// レア度の名前
		const Array<String> rarityNames = { U"ノーマル", U"レア", U"スーパーレア", U"ウルトラレア", U"レジェンド" };
		

		// 各レアリティごとのキャラ
		// 絵文字の配列
		const Array<Array<String>> characters = {
			{ U"😀", U"😃", U"😄" },                     // ノーマル
			{ U"😎", U"🤓", U"🧐" },                     // レア
			{ U"🤠", U"🥳", U"😇" },                     // スーパーレア
			{ U"👽", U"🤖", U"🎃" },                     // ウルトラレア
			{ U"🐉", U"🦄", U"🦖" }                      // レジェンド
		};

		// コンストラクタ
		Gacha(bool isSpecial) {
			
			if (!isSpecial) {
				// 通常ガチャの確率設定
				rarityIndex = rarityDecision(probabilities, randomGacha());

			} else {
				// 最後の1回の確定ガチャの確率設定
				rarityIndex = rarityDecision(lastProbabilities, randomGacha());
			}
			characterIndex = Random(0LL, (int64)(characters[rarityIndex].size() - 1));
		}

		// rarityIndex、characterIndexを取得するメソッド
		int64 getRarityIndex() const {
			return rarityIndex;
		}
		int64 getCharacterIndex() const {
			return characterIndex;
		}
		

	private:
		int64 rarityIndex; // レアリティのインデックス
		int64 characterIndex; // キャラクターのインデックス

		// 0.0 から 1.0 のランダムな実数を生成する関数
		double randomGacha() {
			return Random(0.0, 1.0);
		}

		// 確率に基づいてレアリティを決定する関数
		int64 rarityDecision(Array<double> probs, double randomValue) {
			// probabilities :  各レアリティの出現確率 (和は1)
			double probsSum = 0.0;

			// 確率に基づいてレアリティを決定
			for (size_t i = 0; i < probs.size(); ++i) {
				probsSum += probs[i];
				if (randomValue < probsSum) {
					return i;
				}

			}
			return 0;
		}



};

// ボタンを描画する関数
bool button(const Rect& rect, const Font& font, const String& label) {
	const RoundRect roundRect = rect.rounded(5);

	// マウスカーソルがボタンの上にある場合
	if (roundRect.mouseOver())
	{
		// マウスカーソルを手の形にする
		Cursor::RequestStyle(CursorStyle::Hand);

		// 色を変える
		roundRect
			.drawShadow(Vec2{ 2, 2 }, 12, 0)
			.draw(ColorF{ 0.9, 0.95, 1.0 });

	}
	else {
		// 影と背景の描画
		roundRect
			.drawShadow(Vec2{ 2, 2 }, 12, 0)
			.draw(ColorF{ 0.8, 0.9, 1.0 });
	}

	// 枠の描画
	rect.stretched(-3).rounded(3)
		.drawFrame(2, ColorF{ 0.4, 0.6, 0.8 });

	// テキストの描画
	font(label).drawAt(roundRect.center(), ColorF{ 0.1, 0.1, 0.1 });

	// マウスクリックの検出
	return roundRect.leftClicked();
}

// 背景を描画する関数
void DrawBackground() {
	const double t = Scene::Time() * 15.0;
	const Rect rect = Scene::Rect();
	Quad{ rect.tl(), rect.tr(), rect.br(), rect.bl() }.draw(
		HSV{ t, 0.15, 0.95 },
		HSV{ t + 90, 0.15, 0.95 },
		HSV{ t + 180, 0.15, 0.95 },
		HSV{ t + 270, 0.15, 0.95 }
	);
}

// 共有データの構造体
struct GameData {
	// ガチャの結果を保存する配列
	Array<Gacha> gachaResults;
	
};

// シーンマネージャを用いて画面遷移を管理する
using App = SceneManager<String, GameData>; // シーンのキーは文字列で管理

// タイトルシーン
class Title : public App::Scene {
	public:
		// コンストラクタ
		Title(const InitData& init)
			: IScene{ init } {
		}

		// 更新処理
		void update() override {
			// タイトル表示
			font(U"ガチャシミュレーター").drawAt(400, 100, ColorF{ 0.1, 0.1, 0.1 });
			// 1回ガチャボタン
			if (startButtonRect.leftClicked()) {
				Gacha gacha(false);
				getData().gachaResults.push_back(gacha);
				changeScene(U"Result");
			}

			// 10連ガチャボタン
			if (gacha10ButtonRect.leftClicked()) {
				for (size_t i = 0; i < 10; ++i) {
					Gacha gacha(i == 9);
					getData().gachaResults.push_back(gacha);
					changeScene(U"Result");
				}
			}
		}

		// 描画処理
		void draw() const override {
			// 背景描画
			DrawBackground();

			// タイトルを描画
			font(U"ガチャシミュレーター").drawAt(400, 100, ColorF{ 0.1, 0.1, 0.1 });
			
			button(startButtonRect, font, U"1回ガチャを引く");
			button(gacha10ButtonRect, font, U"10連ガチャを引く");
	}

	private:
		// フォント設定
		const Font font{ FontMethod::MSDF, 40, Typeface::Bold };

		// ボタンの配置をメンバ変数として定義
		const Rect startButtonRect{ 200, 200, 400, 100 };
		const Rect gacha10ButtonRect{ 200, 350, 400, 100 };
};

class Result : public App::Scene {
	public:
		// コンストラクタ
		Result(const InitData& init)
			: IScene{ init } {
		}

		// 更新処理
		void update() override {
			// タイトルに戻るボタン
			if (backButtonRect.leftClicked()) {
				// ガチャ結果をクリア
				getData().gachaResults.clear();
				changeScene(U"Title");
			}
		}

		// 描画処理
		void draw() const override {
			// 背景描画
			DrawBackground();

			// ガチャ結果の表示
			const size_t visibleCount = Min(gachaResults.size(), (size_t)(m_stopwatch.sF() / 0.2));

			for (size_t i = 0; i < visibleCount; ++i) {
				const Gacha& gacha = gachaResults[i];
				// キャラクターの絵文字テクスチャを取得
				drawGachaItem(gacha, i, characterEmojis[i]);
			}

			// タイトルに戻るボタンの描画
			button(backButtonRect, font, U"タイトルに戻る");
			

		
		}
	private:
		// 共有データからガチャ結果を取得
		Array<Gacha> gachaResults = getData().gachaResults;
		// アニメーション用ストップウォッチ
		Stopwatch m_stopwatch{ StartImmediately::Yes };

		// ボタンの配置をメンバ変数として定義
		const Rect backButtonRect{ 300, 520, 200, 50 };

		// フォント設定
		Font font{ FontMethod::MSDF, 22, Typeface::Bold };
		// 各レアリティの色設定
		const Array<ColorF> rarityColors = {
			HSV{ 0, 0, 0.7 },   // ノーマル
			HSV{ 210, 0.8, 1.0 }, // レア
			HSV{ 288, 1.0, 1.0 }, // スーパーレア
			HSV{ 30, 1.0, 1.0 },  // ウルトラレア
			HSV{ 50, 1.0, 1.0 }   // レジェンド
		};

		// ガチャ結果の絵文字アイコンを取得
		const Array<Texture> characterEmojis = getCharacterEmojis();
		

		// 各アイテムごとの描画を管理する関数
		void drawGachaItem(const Gacha& gacha, int64 index, const Texture& emoji) const {
			// ガチャのレアリティ名とキャラクター名を取得
			const String rarityName = gacha.rarityNames[gacha.getRarityIndex()];
			const String character = gacha.characters[gacha.getRarityIndex()][gacha.getCharacterIndex()];


			// ガチャ結果の表示
			double x = 100 + (index % 5) * 150;
			double y = 140 + (index / 5) * 220;
			
			// ボックスを描画
			drawGachaBox(gacha, x, y, emoji);

			// テキストを描画
			String text = rarityName ;
			font(text).drawAt(x, y + 95, rarityColors[gacha.getRarityIndex()]);

			
		}

		// ガチャの結果のボックスとアイコンを描画する関数
		void drawGachaBox(const Gacha& gacha, double x, double y, const Texture& emoji) const {

			// ウルトラレア(3)以上の場合
			if (gacha.getRarityIndex() >= 3) {
				const double time = Scene::Time();
				const double hue = time * 360.0;
				// グラデーション用のRect（角丸なし）
				Rect rect{ (int)x - 65, (int)y - 65, 130, 130 };

				rect.drawShadow(Vec2{ 2, 2 }, 8, 0);
				
				Quad{ rect.tl(), rect.tr(), rect.br(), rect.bl() }
					.draw(
						HSV{ hue, 0.2, 1.0 },
						HSV{ hue + 90, 0.2, 1.0 },
						HSV{ hue + 180, 0.2, 1.0 },
						HSV{ hue + 270, 0.2, 1.0 }
					);

				rect.drawFrame(3, rarityColors[gacha.getRarityIndex()]);

				emoji.resized(100).drawAt(x, y);
				return;
			}

			// ボックスの描画
			RoundRect{ x - 65, y - 65, 130, 130, 15 }
				.drawShadow(Vec2{ 2, 2 }, 8, 0)
				.draw(ColorF{ 0.95, 0.95, 1.0 });
			// 枠の描画
			RoundRect{ x - 65, y - 65, 130, 130, 15 }
				.drawFrame(3, rarityColors[gacha.getRarityIndex()]);
			// キャラクターアイコンの描画（絵文字）
			emoji.resized(100).drawAt(x, y);
			
		}

		// ガチャ結果を絵文字のTexture配列に変換する関数
		Array<Texture> getCharacterEmojis() const {
			Array<Texture> emojis;
			for (const Gacha& g : gachaResults) {
				const Texture emoji{ Emoji{g.characters[g.getRarityIndex()][g.getCharacterIndex()]} };
				emojis.push_back(emoji);
			}
			return emojis;
		}


};


// メイン関数

void Main()
{
	App manager;
	

	// シーンを登録 (名前, クラス)
	manager.add<Title>(U"Title");
	manager.add<Result>(U"Result");
	
	while (System::Update())
	{
		if (!manager.update())
		{
			break;
		}
	}
}
