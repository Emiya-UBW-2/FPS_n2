#pragma once
#include "DXLib_ref/DXLib_ref.h"

/*
class HostPassEffect {
private:
	GraphHandle FarScreen;
	GraphHandle MainScreen;
	GraphHandle NearScreen;
	GraphHandle GaussScreen;
	const int EXTEND = 4;
	bool dof_flag = true;
	bool bloom_flag = true;
	int disp_x = 1920;
	int disp_y = 1080;
public:
	HostPassEffect(const bool& dof_, const bool& bloom_, const int& xd, const int& yd) {
		disp_x = xd;
		disp_y = yd;
		dof_flag = dof_;
		bloom_flag = bloom_;
		FarScreen = GraphHandle::Make(disp_x, disp_y, true);
		MainScreen = GraphHandle::Make(disp_x, disp_y, true);
		NearScreen = GraphHandle::Make(disp_x, disp_y, true);
		GaussScreen = GraphHandle::Make(disp_x / EXTEND, disp_y / EXTEND); //�G�t�F�N�g
	}
	~HostPassEffect() {
	}
	//��ʑ̐[�x�`��
	template <typename T2>
	void dof(
		GraphHandle* buf, GraphHandle& skyhandle, T2 doing,
		DXDraw::cam_info& cams) {
		if (dof_flag) {
			//
			FarScreen.SetDraw_Screen(cams.campos, cams.camvec, cams.camup, cams.fov, cams.far_, 1000000.f);
			skyhandle.DrawGraph(0, 0, FALSE);
			doing();
			//
			MainScreen.SetDraw_Screen(cams.campos, cams.camvec, cams.camup, cams.fov, cams.near_, cams.far_ + 50.f);
			Effekseer_Sync3DSetting();
			GraphFilter(FarScreen.get(), DX_GRAPH_FILTER_GAUSS, 16, 200);
			FarScreen.DrawGraph(0, 0, false);
			UpdateEffekseer3D();
			doing();
			DrawEffekseer3D();
			//
			NearScreen.SetDraw_Screen(cams.campos, cams.camvec, cams.camup, cams.fov, 0.01f, cams.near_ + 0.1f);
			MainScreen.DrawGraph(0, 0, false);
			doing();
		}
		else {
			//
			NearScreen.SetDraw_Screen(cams.campos, cams.camvec, cams.camup, cams.fov, std::clamp(cams.near_, 0.1f, 2000.f), 2000.f);
			Effekseer_Sync3DSetting();
			skyhandle.DrawGraph(0, 0, FALSE);
			UpdateEffekseer3D(); //2.0ms
			doing();
			DrawEffekseer3D();
		}
		buf->SetDraw_Screen();
		NearScreen.DrawGraph(0, 0, false);
	}
	//�u���[���G�t�F�N�g
	void bloom(GraphHandle& BufScreen, const int& level = 255) {
		if (bloom_flag) {
			GraphFilter(BufScreen.get(), DX_GRAPH_FILTER_TWO_COLOR, 245, GetColor(0, 0, 0), 255, GetColor(128, 128, 128), 255);
			GraphFilterBlt(BufScreen.get(), GaussScreen.get(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
			GraphFilter(GaussScreen.get(), DX_GRAPH_FILTER_GAUSS, 16, 1000);
			SetDrawMode(DX_DRAWMODE_BILINEAR);
			SetDrawBlendMode(DX_BLENDMODE_ADD, level);
			GaussScreen.DrawExtendGraph(0, 0, disp_x, disp_y, false);
			GaussScreen.DrawExtendGraph(0, 0, disp_x, disp_y, false);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
	}

public:
};
*/

class HostPassEffect {
private:
	GraphHandle FarScreen_;		//�`��X�N���[��
	GraphHandle NearFarScreen_;	//�`��X�N���[��
	GraphHandle NearScreen_;	//�`��X�N���[��
	GraphHandle GaussScreen_;	//�`��X�N���[��
	GraphHandle BufScreen;		//�`��X�N���[��
	GraphHandle BufScreen_;		//�`��X�N���[��
	GraphHandle SkyScreen;		//��`��

	int EXTEND = 4;
	bool dof_flag = true;
	bool bloom_flag = true;
	int disp_x = deskx;
	int disp_y = desky;
public:
	GraphHandle MAIN_Screen;	//�`��X�N���[��

	HostPassEffect(const bool& dof_, const bool& bloom_, const int& xd, const int& yd) {
		disp_x = xd;
		disp_y = yd;
		//DoF�p
		dof_flag = dof_;
		SkyScreen = GraphHandle::Make(disp_x, disp_y, false);						//��`��
		FarScreen_ = GraphHandle::Make(disp_x, disp_y, true);						//�`��X�N���[��
		NearFarScreen_ = GraphHandle::Make(disp_x, disp_y, true);					//�`��X�N���[��
		NearScreen_ = GraphHandle::Make(disp_x, disp_y, true);						//�`��X�N���[��
		//�u���[���p
		bloom_flag = bloom_;
		GaussScreen_ = GraphHandle::Make(disp_x / EXTEND, disp_y / EXTEND, true);	//�`��X�N���[��
		BufScreen_ = GraphHandle::Make(disp_x, disp_y, true);						//�`��X�N���[��
		//�ŏI�`��p
		BufScreen = GraphHandle::Make(disp_x, disp_y, true);						//�`��X�N���[��
		MAIN_Screen = GraphHandle::Make(disp_x, disp_y, true);						//�`��X�N���[��
	}
	~HostPassEffect() {
	}
private:
	//�u���[���G�t�F�N�g
	void buf_bloom(const int& level = 255) {
		BufScreen.SetDraw_Screen(false);
		{
			if (bloom_flag) {
				GraphFilterBlt(BufScreen.get(), BufScreen_.get(), DX_GRAPH_FILTER_TWO_COLOR, 245, GetColor(0, 0, 0), 255, GetColor(128, 128, 128), 255);
				GraphFilterBlt(BufScreen_.get(), GaussScreen_.get(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
				GraphFilter(GaussScreen_.get(), DX_GRAPH_FILTER_GAUSS, 16, 1000);
				SetDrawMode(DX_DRAWMODE_BILINEAR);
				SetDrawBlendMode(DX_BLENDMODE_ADD, level);
				GaussScreen_.DrawExtendGraph(0, 0, disp_x, disp_y, true);
				GaussScreen_.DrawExtendGraph(0, 0, disp_x, disp_y, true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
		}
	}
	//��ʑ̐[�x�`��
	template <typename T1, typename T2>
	void near_dof(T1 sky_doing, T2 doing, DXDraw::cam_info& cams, bool update_effekseer = true) {
		if (dof_flag) {
			//��
			SkyScreen.SetDraw_Screen(cams.campos - cams.camvec, VGet(0, 0, 0), cams.camup, cams.fov, 1000.0f, 5000.0f);
			{
				sky_doing();
			}
			//������
			FarScreen_.SetDraw_Screen(cams.campos, cams.camvec, cams.camup, cams.fov, cams.far_, 1000000.f);
			{
				SkyScreen.DrawGraph(0, 0, FALSE);
				doing();
			}
			//����
			NearFarScreen_.SetDraw_Screen(cams.campos, cams.camvec, cams.camup, cams.fov, cams.near_, cams.far_);
			{
				Effekseer_Sync3DSetting();
				GraphFilter(FarScreen_.get(), DX_GRAPH_FILTER_GAUSS, 16, 200);
				FarScreen_.DrawGraph(0, 0, false);
				if (update_effekseer) {
					UpdateEffekseer3D();
				}
				doing();
				DrawEffekseer3D();
			}
			//����
			NearScreen_.SetDraw_Screen(cams.campos, cams.camvec, cams.camup, cams.fov, 0.1f, 0.1f + cams.near_);
			{
				NearFarScreen_.DrawGraph(0, 0, false);
				doing();
				DrawEffekseer3D();
			}
		}
	}
	//���ɉ��������`��
	template <typename T1, typename T2>
	void near_nomal(T1 sky_doing, T2 doing, DXDraw::cam_info& cams, bool update_effekseer = true) {
		//��
		SkyScreen.SetDraw_Screen(cams.campos - cams.camvec, VGet(0, 0, 0), cams.camup, cams.fov, 1000.0f, 5000.0f);
		{
			sky_doing();
		}
		NearScreen_.SetDraw_Screen(cams.campos, cams.camvec, cams.camup, cams.fov, cams.near_, cams.far_);
		{
			Effekseer_Sync3DSetting();
			SkyScreen.DrawGraph(0, 0, FALSE);
			if (update_effekseer) {
				UpdateEffekseer3D();
			}
			doing();
			DrawEffekseer3D();
		}
	}
public:
	template <typename T1 ,typename T2>
	void BUF_draw(T1 sky_doing, T2 doing, DXDraw::cam_info& cams, bool update_effekseer = true) {
		//near�ɕ`��
		if (dof_flag) {
			near_dof(sky_doing, doing, cams, update_effekseer);
		}
		else {
			near_nomal(sky_doing, doing, cams, update_effekseer);
		}
		//���ʕ`��
		BufScreen.SetDraw_Screen();
		{
			NearScreen_.DrawGraph(0, 0, false);
		}
	}
	//
	void MAIN_draw() {
		//buf�ɕ`��
		if (bloom_flag) {
			buf_bloom(255);//�u���[��
		}
		//���ʕ`��
		MAIN_Screen.SetDraw_Screen();
		{
			BufScreen.DrawGraph(0, 0, false);
		}
	}
	//
};
