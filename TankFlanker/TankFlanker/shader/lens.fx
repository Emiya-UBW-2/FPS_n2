//ピクセルシェーダーV3.0以上。
//V2.0に対応させるには位置情報と配列サイズを2つに減らす。

// ピクセルシェーダーの入力
struct PS_INPUT
{
	float4 DiffuseColor       : COLOR0;
	float4 SpecularColor      : COLOR1;
	float2 TextureCoord0      : TEXCOORD0; //テクスチャ座標
	float2 TextureCoord1      : TEXCOORD1; //座標(ビュー空間)
};

// ピクセルシェーダーの出力
struct PS_OUTPUT
{
	float4 Output             : COLOR0;
};

// 描画するテクスチャ
sampler Texture : register(s0);

//外部変数(生成される全ブラックホールに適応されます。個別にパラメータを設定できるように改良できるかも)
float scr_w : register(c0); //画面幅
float scr_h : register(c1); //画面高さ
float mass : register(c2); //重さ
float max_dis : register(c3); //影響範囲
float bh_flag : register(c4); //ブラックホールフラグ
//位置定義取得(ここはもっとまとめられそうな気が……)
float pos_num : register(c16);
float2 pos_0 : register(c17);
float2 pos_1 : register(c18);
float2 pos_2 : register(c19);
float2 pos_3 : register(c20);
float2 pos_4 : register(c21);
float2 pos_5 : register(c22);
float2 pos_6 : register(c23);
float2 pos_7 : register(c24);
float2 pos_8 : register(c25);
float2 pos_9 : register(c26);
float2 pos_10 : register(c27);
float2 pos_11 : register(c28);
float2 pos_12 : register(c29);
float2 pos_13 : register(c30);
float2 pos_14 : register(c31);
float2 pos_15 : register(c32);
float2 pos_16 : register(c33);
float2 pos_17 : register(c34);
float2 pos_18 : register(c35);
float2 pos_19 : register(c36);
float2 pos_20 : register(c37);
float2 pos_21 : register(c38);
float2 pos_22 : register(c39);
float2 pos_23 : register(c40);
float2 pos_24 : register(c41);
float2 pos_25 : register(c42);
float2 pos_26 : register(c43);
float2 pos_27 : register(c44);
float2 pos_28 : register(c45);
float2 pos_29 : register(c46);
float2 pos_30 : register(c47);

//外部から取得した位置情報を配列にセットする関数(参照渡しやポインタ渡しのようにしたかったらoutをつければ良い)
void SetPosArray(out float2 pos_array[31]) {
	pos_array[0] = pos_0;
	pos_array[1] = pos_1;
	pos_array[2] = pos_2;
	pos_array[3] = pos_3;
	pos_array[4] = pos_4;
	pos_array[5] = pos_5;
	pos_array[6] = pos_6;
	pos_array[7] = pos_7;
	pos_array[8] = pos_8;
	pos_array[9] = pos_9;
	pos_array[10] = pos_10;
	pos_array[11] = pos_11;
	pos_array[12] = pos_12;
	pos_array[13] = pos_13;
	pos_array[14] = pos_14;
	pos_array[15] = pos_15;
	pos_array[16] = pos_16;
	pos_array[17] = pos_17;
	pos_array[18] = pos_18;
	pos_array[19] = pos_19;
	pos_array[20] = pos_20;
	pos_array[21] = pos_21;
	pos_array[22] = pos_22;
	pos_array[23] = pos_23;
	pos_array[24] = pos_24;
	pos_array[25] = pos_25;
	pos_array[26] = pos_26;
	pos_array[27] = pos_27;
	pos_array[28] = pos_28;
	pos_array[29] = pos_29;
	pos_array[30] = pos_30;
}

//ブラックホールによる屈折の計算を行う関数(ブラックホールの位置、計算対象のピクセル位置)
float3 BlackHoleCalc(in float2 pos, in float2 pix) {
	float3 out_pos;
	float2 pos_pix_vec;
	float dis = sqrt(pow(pos.x - pix.x, 2) + pow(pos.y - pix.y, 2));
	out_pos.z = 0;
	if (dis > max_dis) {
		out_pos.x = 0;
		out_pos.y = 0;
	}
	else {
		pos_pix_vec = pix - pos;
		float g = mass / pow(dis, 2);
		float mag = 1 - pow(g, 2);
		pos_pix_vec *= mag;
		float2 cpos = pos + pos_pix_vec;
		out_pos.x = (cpos - pix).x;
		out_pos.y = (cpos - pix).y;
		if (bh_flag == 1.0) { out_pos.z = mag; }
	}
	return out_pos;
}

//main関数。名前の通りこれが実行される。
PS_OUTPUT main(PS_INPUT PSInput)
{
	float2 pos_array[31];
	SetPosArray(pos_array);
	PS_OUTPUT PSOutput;
	float4 TextureColor;
	float2 pix;
	pix.x = PSInput.TextureCoord0.x*scr_w; //ピクセル座標に変換する
	pix.y = PSInput.TextureCoord0.y*scr_h;
	float2 CalcPos = pix;
	float3 calc_pos = { 0,0,0 };

	//セットされたブラックホールの数だけ計算する
	for (float i = 0; i < pos_num; ++i) {
		calc_pos += BlackHoleCalc(pos_array[i], pix);
	}

	CalcPos.x += calc_pos.x;
	CalcPos.y += calc_pos.y;

	CalcPos.x /= scr_w; //ピクセル座標から戻す
	CalcPos.y /= scr_h;

	ブラックホールの内側は黒くする。
		TextureColor = tex2D(Texture, CalcPos);
	if (calc_pos.z < 0) {
		TextureColor.r = 0;
		TextureColor.b = 0;
		TextureColor.g = 0;
	}
	PSOutput.Output = TextureColor;
	return PSOutput;
}