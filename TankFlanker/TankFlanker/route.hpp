#pragma once
class main_c : Mainclass {
	bool dof_e = false;
	bool bloom_e = false;
	bool shadow_e = false;
	bool useVR_e = true;
	float fov_pc = 45.f;

	DXDraw::cam_info cam_easy;
	DXDraw::cam_info cam_easy2;

	GraphHandle outScreen2;
	//����
	VECTOR_ref campos_TPS;			//�J����
	switchs TPS;					//����X�C�b�`
	float xrad_p = 0.f;				//�}�E�X�G�C���p�ϐ��m��
	//�f�[�^
	MV1 body_obj;					//�g�̃��f��
	MV1 body_col;					//�g�̃��f��
	std::vector<Gun> gun_data;		//GUN�f�[�^
	std::vector<Chara> chara;		//�L����
	std::vector<Items> item_data;	//�E����A�C�e��
	//�ݒ�
	bool oldv = false;
	bool oldv_1 = false;
	bool oldv_2 = false;
	bool oldv_3 = false;

	bool start_c = true;
	bool ending = true;
	//
public:
	main_c() {
		//�ݒ�ǂݍ���
		{
			SetOutApplicationLogValidFlag(FALSE);	//log
			int mdata = FileRead_open("data/setting.txt", FALSE);
			dof_e = getparams::_bool(mdata);
			bloom_e = getparams::_bool(mdata);
			shadow_e = getparams::_bool(mdata);
			useVR_e = getparams::_bool(mdata);
			fov_pc = getparams::_float(mdata);
			FileRead_close(mdata);
			SetOutApplicationLogValidFlag(TRUE);	//log
		}
		auto Drawparts = std::make_unique<DXDraw>("FPS_n2", FRAME_RATE, useVR_e, shadow_e);										//�ėp
		auto UIparts = std::make_unique<UI>();																					//UI
		auto Debugparts = std::make_unique<DeBuG>(FRAME_RATE);																	//�f�o�b�O
		auto Hostpassparts = std::make_unique<HostPassEffect>(dof_e, bloom_e, Drawparts->disp_x, Drawparts->disp_y);			//�z�X�g�p�X�G�t�F�N�g(VR�A�t���X�N���[�����p)
		auto Hostpass2parts = std::make_unique<HostPassEffect>(dof_e, bloom_e, Drawparts->out_disp_x, Drawparts->out_disp_y);	//�z�X�g�p�X�G�t�F�N�g(�t���X�N���[������)
		auto mapparts = std::make_unique<Mapclass>();																			//MAP
		//model
		MV1::Load("data/model/body/model.mv1", &this->body_obj, true);															//�g��
		MV1::Load("data/model/body/col.mv1", &this->body_col, true);															//�g��
		outScreen2 = GraphHandle::Make(Drawparts->out_disp_x, Drawparts->out_disp_y, true);										//�`��X�N���[��
		//GUN�f�[�^
		{
			this->gun_data.resize(1);
			this->gun_data[0].mod.set("GUN");
		}
		UIparts->load_window("�e���f��");						//���[�h���1
		//GUN�f�[�^�擾2
		for (auto& g : this->gun_data) {
			g.id = &g - &this->gun_data[0];
			g.set_data();
		}
		UIparts->load_window("�e�f�[�^");						//���[�h���2
		do {
			//�L�����ݒ�
			size_t sel_g = 0;
			chara.resize(3);
			auto& mine = chara[0];
			//���@�Z�b�g
			mine.Ready_chara(this->gun_data, sel_g, this->body_obj,this->body_col);
			mine.Set_chara_Position(VGet(0.0f, 4.0f, 0.f), MATRIX_ref::RotY(DX_PI_F));
			mine.Set_chara();
			//���̑�
			chara[1].Ready_chara(this->gun_data, sel_g, this->body_obj, this->body_col);
			chara[1].Set_chara_Position(VGet(0.0f, 4.0f, 2.f), MATRIX_ref::RotY(DX_PI_F*2));
			chara[1].Set_chara();

			chara[2].Ready_chara(this->gun_data, sel_g, this->body_obj, this->body_col);
			chara[2].Set_chara_Position(VGet(0.0f, 4.0f, 4.f), MATRIX_ref::RotY(DX_PI_F*2));
			chara[2].Set_chara();

			//�}�b�v�ǂݍ���
			mapparts->Ready_map("data/map");			//mapparts->Ready_map("data/new");
			UIparts->load_window("�}�b�v");
			mapparts->Set_map("data/maps/set.txt", this->item_data, this->gun_data);
			//���C�e�B���O
			Drawparts->Set_Light_Shadow(mapparts->map_col_get().mesh_maxpos(0), mapparts->map_col_get().mesh_minpos(0), VGet(0.5f, -0.5f, 0.5f), [&] {mapparts->map_get().DrawModel(); });
			//�`�悷����̂��w�肷��(��)
			auto draw_by_shadow = [&] {
				Drawparts->Draw_by_Shadow(
					[&] {
					mapparts->map_get().DrawModel();
					for (auto& c : this->chara) {
						c.Draw_chara();
					}
					for (auto& g : this->item_data) {
						g.Draw_item(this->chara[0]);
					}
					//�e�e
					SetFogEnable(FALSE);
					SetUseLighting(FALSE);
					for (auto& c : this->chara) {
						c.Draw_ammo();
					}
					SetUseLighting(TRUE);
					SetFogEnable(TRUE);
				});
			};
			//�J�n
			{
				//��
				mapparts->Start_map();
				//�v���C���[����ϐ��Q
				this->xrad_p = 0.f;									//�}�E�X�G�C��
				this->TPS.ready(false);
				for (auto& c : chara) {
					c.ads.ready(false);
					c.running = false;
					c.squat.ready(false);
				}
				mine.start_c = true;
				SetMousePoint(deskx / 2, desky / 2);
				//
				while (ProcessMessage() == 0) {
					const auto waits = GetNowHiPerformanceCount();
					Debugparts->put_way();
					{
						//���W�擾
						if (Drawparts->tracker_num.size() > 0) {
							auto& ptr_ = (*Drawparts->get_device())[Drawparts->tracker_num[0]];
							Drawparts->GetDevicePositionVR(Drawparts->tracker_num[0], &mine.pos_WAIST, &mine.mat_WAIST);
							if (mine.start_c || (ptr_.turn && ptr_.now) != oldv_1) {
								mine.pos_WAIST_rep = VGet(mine.pos_WAIST.x(), 0.f, mine.pos_WAIST.z());
							}
							oldv_1 = ptr_.turn && ptr_.now;
							mine.pos_WAIST = mine.pos_WAIST - mine.pos_WAIST_rep;
						}
						//�v���C���[����
						{
							//cpu
							for (auto& c : chara) {
								if (&c - &chara[0] >= (Drawparts->use_vr ? 2 : 1)) {
									c.wkey = (CheckHitKey(KEY_INPUT_W) != 0);
									c.skey = (CheckHitKey(KEY_INPUT_S) != 0);
									c.akey = (CheckHitKey(KEY_INPUT_A) != 0);
									c.dkey = (CheckHitKey(KEY_INPUT_D) != 0);
									c.running = (CheckHitKey(KEY_INPUT_LSHIFT) != 0);
									c.squat.get_in(CheckHitKey(KEY_INPUT_C) != 0);
									auto qkey = (CheckHitKey(KEY_INPUT_Q) != 0);
									auto ekey = (CheckHitKey(KEY_INPUT_E) != 0);

									int32_t x_m= deskx / 2, y_m= desky / 2;

									//HMD_mat
									{
										c.mat_HMD *= MATRIX_ref::RotAxis(c.mat_HMD.zvec(), c.body_zrad).Inverse();
										if (qkey) {
											easing_set(&c.body_zrad, deg2rad(-30), 0.9f);
										}
										else if (ekey) {
											easing_set(&c.body_zrad, deg2rad(30), 0.9f);
										}
										else {
											easing_set(&c.body_zrad, 0.f, 0.9f);
										}
										c.mat_HMD *= MATRIX_ref::RotAxis(c.mat_HMD.zvec(), c.body_zrad);

										c.mat_HMD = MATRIX_ref::RotX(-this->xrad_p)*c.mat_HMD;
										this->xrad_p = std::clamp(this->xrad_p - deg2rad(std::clamp(y_m - desky / 2, -120, 120))*0.1f, deg2rad(-80), deg2rad(60));
										c.mat_HMD *= MATRIX_ref::RotY(deg2rad(std::clamp(x_m - deskx / 2, -120, 120))*0.1f);
										c.mat_HMD = MATRIX_ref::RotX(this->xrad_p)*c.mat_HMD;
									}
									//�ړ�
									{
										if (c.running) {
											c.squat.first = false;
										}
										if (c.ads.first) {
											c.running = false;
										}
										if (!c.wkey && !c.skey && !c.akey && !c.dkey) {
											c.running = false;
										}
										if (c.running) {
											c.skey = false;
											c.akey = false;
											c.dkey = false;
										}
										auto speed = (c.running ? 6.f : ((c.ads.first ? 2.f : 4.f)*(c.squat.first ? 0.4f : 1.f))) / GetFPS();
										VECTOR_ref zv_t = c.mat_HMD.zvec();
										zv_t.y(0.f);
										zv_t = zv_t.Norm();

										VECTOR_ref xv_t = c.mat_HMD.xvec();
										xv_t.y(0.f);
										xv_t = xv_t.Norm();

										if (c.wkey) {
											easing_set(&c.add_pos_buf, zv_t*-speed, 0.95f);
										}
										if (c.skey) {
											easing_set(&c.add_pos_buf, zv_t*speed, 0.95f);
										}
										if (c.akey) {
											easing_set(&c.add_pos_buf, xv_t*speed, 0.95f);
										}
										if (c.dkey) {
											easing_set(&c.add_pos_buf, xv_t*-speed, 0.95f);
										}
										if (!c.wkey && !c.skey && !c.akey && !c.dkey) {
											easing_set(&c.add_pos_buf, VGet(0, 0, 0), 0.95f);
										}
										if (c.add_ypos == 0.f) {
											c.add_pos = c.add_pos_buf;
										}
										else {
											easing_set(&c.add_pos, VGet(0, 0, 0), 0.995f);
										}
									}
									//����
									{
										//�}�K�W���擾
										c.down_mag = true;
										//������(���N���b�N)
										easing_set(&c.obj_gun.get_anime(2).per, float(((GetMouseInput() & MOUSE_INPUT_LEFT) != 0) && !c.running), 0.5f);
										//ADS
										c.ads.first = ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0) && (!c.reloadf);
										//�Z���N�^�[(���{�^��)
										c.selkey.get_in((GetMouseInput() & MOUSE_INPUT_MIDDLE) != 0);
										//�}�O�L���b�`(R�L�[)
										easing_set(&c.obj_gun.get_anime(5).per, float((CheckHitKey(KEY_INPUT_R) != 0)), 0.5f);
										//�e�擾
										c.getmag.get_in(CheckHitKey(KEY_INPUT_F) != 0);
									}
								}
							}
							//chara
							if (Drawparts->use_vr) {
								//HMD_mat
								{
									//+���_�擾
									auto& ptr_ = (*Drawparts->get_device())[Drawparts->get_hmd_num()];
									Drawparts->GetDevicePositionVR(Drawparts->get_hmd_num(), &mine.pos_HMD, &mine.mat_HMD);
									if (mine.start_c || (ptr_.turn && ptr_.now) != oldv) {
										mine.rec_HMD = VGet(mine.pos_HMD.x(), 0.f, mine.pos_HMD.z());
									}
									oldv = ptr_.turn && ptr_.now;
								}
								//�ړ�
								if (Drawparts->get_hand2_num() != -1) {
									auto& ptr_ = (*Drawparts->get_device())[Drawparts->get_hand2_num()];
									if (ptr_.turn && ptr_.now) {
										if ((ptr_.on[0] & BUTTON_TOUCHPAD) != 0) {
											//running
											mine.wkey = false;
											mine.skey = false;
											mine.akey = false;
											mine.dkey = false;
											mine.running = false;//(ptr_.on[0] & BUTTON_TOUCHPAD) != 0;
											auto speed = (mine.running ? 6.f : 2.f) / GetFPS();
											if (mine.running) {
												easing_set(&mine.add_pos_buf, (mine.mat_HMD.zvec()*ptr_.touch.y())*speed, 0.95f);
											}
											else {
												easing_set(&mine.add_pos_buf, (mine.mat_HMD.zvec()*ptr_.touch.y() + mine.mat_HMD.xvec()*ptr_.touch.x())*speed, 0.95f);
											}
										}
										else {
											easing_set(&mine.add_pos_buf, VGet(0, 0, 0), 0.95f);
										}
										if (mine.add_ypos == 0.f) {
											if ((ptr_.on[0] & BUTTON_SIDE) != 0) {
												mine.add_ypos = 0.05f;
											}
											mine.add_pos = mine.add_pos_buf;
										}
										else {
											easing_set(&mine.add_pos, VGet(0, 0, 0), 0.995f);
										}
									}
								}
								//����
								{
									if (Drawparts->get_hand1_num() != -1) {
										auto& ptr_ = (*Drawparts->get_device())[Drawparts->get_hand1_num()];
										if (ptr_.turn && ptr_.now) {
											//������
											easing_set(&mine.obj_gun.get_anime(2).per, float((ptr_.on[0] & BUTTON_TRIGGER) != 0), 0.5f);
											//�}�O�L���b�`
											easing_set(&mine.obj_gun.get_anime(5).per, float((ptr_.on[0] & BUTTON_SIDE) != 0), 0.5f);
											//�Z���N�^�[
											mine.selkey.get_in(((ptr_.on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_.touch.x() > 0.5f && ptr_.touch.y() < 0.5f&&ptr_.touch.y() > -0.5f));
										}
									}
									if (Drawparts->get_hand2_num() != -1) {
										auto& ptr_ = (*Drawparts->get_device())[Drawparts->get_hand2_num()];
										if (ptr_.turn && ptr_.now) {
											//�}�K�W���擾
											mine.down_mag |= (((ptr_.on[0] & BUTTON_TRIGGER) != 0) && (mine.gun_stat[mine.ptr_now->id].mag_in.size() >= 1));

											mine.getmag.get_in((ptr_.on[0] & BUTTON_TOPBUTTON_B) != 0);
										}
									}
								}
								//2P
								{
									auto& ct = chara[1];
									//HMD_mat
									{
										auto qkey = (CheckHitKey(KEY_INPUT_Q) != 0);
										auto ekey = (CheckHitKey(KEY_INPUT_E) != 0);
										ct.mat_HMD *= MATRIX_ref::RotAxis(ct.mat_HMD.zvec(), ct.body_zrad).Inverse();
										if (qkey) {
											easing_set(&ct.body_zrad, deg2rad(-30), 0.9f);
										}
										else if (ekey) {
											easing_set(&ct.body_zrad, deg2rad(30), 0.9f);
										}
										else {
											easing_set(&ct.body_zrad, 0.f, 0.9f);
										}
										ct.mat_HMD *= MATRIX_ref::RotAxis(ct.mat_HMD.zvec(), ct.body_zrad);
										int32_t x_m, y_m;
										GetMousePoint(&x_m, &y_m);
										ct.mat_HMD = MATRIX_ref::RotX(-this->xrad_p)*ct.mat_HMD;
										this->xrad_p = std::clamp(this->xrad_p - deg2rad(std::clamp(y_m - desky / 2, -120, 120))*0.1f, deg2rad(-80), deg2rad(60));
										ct.mat_HMD *= MATRIX_ref::RotY(deg2rad(std::clamp(x_m - deskx / 2, -120, 120))*0.1f);
										ct.mat_HMD = MATRIX_ref::RotX(this->xrad_p)*ct.mat_HMD;
										SetMousePoint(deskx / 2, desky / 2);
										SetMouseDispFlag(FALSE);
									}
									//�ړ�
									{
										ct.wkey = (CheckHitKey(KEY_INPUT_W) != 0);
										ct.skey = (CheckHitKey(KEY_INPUT_S) != 0);
										ct.akey = (CheckHitKey(KEY_INPUT_A) != 0);
										ct.dkey = (CheckHitKey(KEY_INPUT_D) != 0);
										ct.running = (CheckHitKey(KEY_INPUT_LSHIFT) != 0);
										ct.squat.get_in(CheckHitKey(KEY_INPUT_C) != 0);
										if (ct.running) {
											ct.squat.first = false;
										}
										if (ct.ads.first) {
											ct.running = false;
										}
										if (!ct.wkey && !ct.skey && !ct.akey && !ct.dkey) {
											ct.running = false;
										}
										if (ct.running) {
											ct.skey = false;
											ct.akey = false;
											ct.dkey = false;
										}
										auto speed = (ct.running ? 6.f : ((ct.ads.first ? 2.f : 4.f)*(ct.squat.first ? 0.4f : 1.f))) / GetFPS();
										VECTOR_ref zv_t = ct.mat_HMD.zvec();
										zv_t.y(0.f);
										zv_t = zv_t.Norm();

										VECTOR_ref xv_t = ct.mat_HMD.xvec();
										xv_t.y(0.f);
										xv_t = xv_t.Norm();

										if (ct.wkey) {
											easing_set(&ct.add_pos_buf, zv_t*-speed, 0.95f);
										}
										if (ct.skey) {
											easing_set(&ct.add_pos_buf, zv_t*speed, 0.95f);
										}
										if (ct.akey) {
											easing_set(&ct.add_pos_buf, xv_t*speed, 0.95f);
										}
										if (ct.dkey) {
											easing_set(&ct.add_pos_buf, xv_t*-speed, 0.95f);
										}
										if (!ct.wkey && !ct.skey && !ct.akey && !ct.dkey) {
											easing_set(&ct.add_pos_buf, VGet(0, 0, 0), 0.95f);
										}
										if (ct.add_ypos == 0.f) {
											ct.add_pos = ct.add_pos_buf;
										}
										else {
											easing_set(&ct.add_pos, VGet(0, 0, 0), 0.995f);
										}
									}
									//����
									{
										//�}�K�W���擾
										ct.down_mag = true;
										//������(���N���b�N)
										easing_set(&ct.obj_gun.get_anime(2).per, float(((GetMouseInput() & MOUSE_INPUT_LEFT) != 0) && !ct.running), 0.5f);
										//ADS
										ct.ads.first = ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0) && (!ct.reloadf);
										//�Z���N�^�[(���{�^��)
										ct.selkey.get_in((GetMouseInput() & MOUSE_INPUT_MIDDLE) != 0);
										//�}�O�L���b�`(R�L�[)
										easing_set(&ct.obj_gun.get_anime(5).per, float((CheckHitKey(KEY_INPUT_R) != 0)), 0.5f);
										//�e�擾
										ct.getmag.get_in(CheckHitKey(KEY_INPUT_F) != 0);
									}
								}
							}
							else {
								//HMD_mat
								{
									auto qkey = (CheckHitKey(KEY_INPUT_Q) != 0);
									auto ekey = (CheckHitKey(KEY_INPUT_E) != 0);
									mine.mat_HMD *= MATRIX_ref::RotAxis(mine.mat_HMD.zvec(), mine.body_zrad).Inverse();
									if (qkey) {
										easing_set(&mine.body_zrad, deg2rad(-30), 0.9f);
									}
									else if (ekey) {
										easing_set(&mine.body_zrad, deg2rad(30), 0.9f);
									}
									else {
										easing_set(&mine.body_zrad, 0.f, 0.9f);
									}
									mine.mat_HMD *= MATRIX_ref::RotAxis(mine.mat_HMD.zvec(), mine.body_zrad);
									int32_t x_m, y_m;
									GetMousePoint(&x_m, &y_m);
									mine.mat_HMD = MATRIX_ref::RotX(-this->xrad_p)*mine.mat_HMD;
									this->xrad_p = std::clamp(this->xrad_p - deg2rad(std::clamp(y_m - desky / 2, -120, 120))*0.1f, deg2rad(-80), deg2rad(60));
									mine.mat_HMD *= MATRIX_ref::RotY(deg2rad(std::clamp(x_m - deskx / 2, -120, 120))*0.1f);
									mine.mat_HMD = MATRIX_ref::RotX(this->xrad_p)*mine.mat_HMD;
									SetMousePoint(deskx / 2, desky / 2);
									SetMouseDispFlag(FALSE);
								}
								//�ړ�
								{
									mine.wkey = (CheckHitKey(KEY_INPUT_W) != 0);
									mine.skey = (CheckHitKey(KEY_INPUT_S) != 0);
									mine.akey = (CheckHitKey(KEY_INPUT_A) != 0);
									mine.dkey = (CheckHitKey(KEY_INPUT_D) != 0);
									mine.running = (CheckHitKey(KEY_INPUT_LSHIFT) != 0);
									mine.squat.get_in(CheckHitKey(KEY_INPUT_C) != 0);
									if (mine.running) {
										mine.squat.first = false;
									}
									if (mine.ads.first) {
										mine.running = false;
									}
									if (!mine.wkey && !mine.skey && !mine.akey && !mine.dkey) {
										mine.running = false;
									}
									if (mine.running) {
										mine.skey = false;
										mine.akey = false;
										mine.dkey = false;
									}
									auto speed = (mine.running ? 6.f : ((mine.ads.first ? 2.f : 4.f)*(mine.squat.first ? 0.4f : 1.f))) / GetFPS();
									VECTOR_ref zv_t = mine.mat_HMD.zvec();
									zv_t.y(0.f);
									zv_t = zv_t.Norm();

									VECTOR_ref xv_t = mine.mat_HMD.xvec();
									xv_t.y(0.f);
									xv_t = xv_t.Norm();

									if (mine.wkey) {
										easing_set(&mine.add_pos_buf, zv_t*-speed, 0.95f);
									}
									if (mine.skey) {
										easing_set(&mine.add_pos_buf, zv_t*speed, 0.95f);
									}
									if (mine.akey) {
										easing_set(&mine.add_pos_buf, xv_t*speed, 0.95f);
									}
									if (mine.dkey) {
										easing_set(&mine.add_pos_buf, xv_t*-speed, 0.95f);
									}
									if (!mine.wkey && !mine.skey && !mine.akey && !mine.dkey) {
										easing_set(&mine.add_pos_buf, VGet(0, 0, 0), 0.95f);
									}
									if (mine.add_ypos == 0.f) {
										mine.add_pos = mine.add_pos_buf;
									}
									else {
										easing_set(&mine.add_pos, VGet(0, 0, 0), 0.995f);
									}
								}
								//����
								{
									//�}�K�W���擾
									mine.down_mag = true;
									//������(���N���b�N)
									easing_set(&mine.obj_gun.get_anime(2).per, float(((GetMouseInput() & MOUSE_INPUT_LEFT) != 0) && !mine.running), 0.5f);
									//ADS
									mine.ads.first = ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0) && (!mine.reloadf);
									//�Z���N�^�[(���{�^��)
									mine.selkey.get_in((GetMouseInput() & MOUSE_INPUT_MIDDLE) != 0);
									//�}�O�L���b�`(R�L�[)
									easing_set(&mine.obj_gun.get_anime(5).per, float((CheckHitKey(KEY_INPUT_R) != 0)), 0.5f);
									//�e�擾
									mine.getmag.get_in(CheckHitKey(KEY_INPUT_F) != 0);
								}
							}
						}
						if (Drawparts->use_vr) {
							easing_set(&this->campos_TPS, VGet(-0.35f, 0.125f, 3.f), 0.95f);
						}
						//
						for (auto& c : chara) {
							//�ǂ��̑��̔���
							{
								VECTOR_ref pos_t2 = (c.pos + (VGet(c.pos_HMD.x(), 0.f, c.pos_HMD.z())) - c.rec_HMD);
								VECTOR_ref pos_t = (c.pos + (VGet(c.pos_HMD.x(), 0.f, c.pos_HMD.z())) - c.rec_HMD);
								pos_t += c.add_pos;
								//��
								{
									mapparts->map_col_wall(pos_t2, &pos_t);
									if ((c.add_pos - (pos_t - pos_t2)).size() != 0.f) {
										c.add_pos = pos_t - pos_t2;
										if (c.add_ypos == 0.f) {
											c.add_pos_buf = c.add_pos;
										}
									}
								}
								//����
								{
									auto pp = mapparts->map_col_line(pos_t + VGet(0, 1.6f, 0), pos_t, 0);
									if (c.add_ypos <= 0.f && pp.HitFlag == 1) {
										if (VECTOR_ref(VGet(0, 1.f, 0.f)).dot(pp.Normal) >= cos(deg2rad(30))) {
											pos_t = pp.HitPosition;
										}
										else {
											//�u���b�N����x�N�g��
											auto v_t = VECTOR_ref(pp.Normal);
											v_t.y(0);
											v_t = v_t.Norm();
											//
											pos_t -= c.add_pos;
											c.add_pos += v_t * c.add_pos.cross(v_t.cross(c.add_pos).Norm()).size();
											if (c.add_ypos == 0.f) {
												c.add_pos_buf = c.add_pos;
											}
											pos_t += c.add_pos;
										}
										c.add_ypos = 0.f;
									}
									else {
										pos_t.yadd(c.add_ypos);
										c.add_ypos += M_GR / std::powf(GetFPS(), 2.f);
										//���A
										if (pos_t.y() <= -5.f) {
											pos_t = c.spawn_pos;
											c.add_ypos = 0.f;
											c.body.SetMatrix(MATRIX_ref::Mtrans(pos_t));
											c.body.PhysicsResetState();
										}
									}
								}
								//���f
								c.pos = pos_t - (VGet(c.pos_HMD.x(), 0.f, c.pos_HMD.z())) + c.rec_HMD;
							}
							//pos
							if (Drawparts->use_vr && (&c == &mine)) {
								{
									VECTOR_ref v_ = c.mat_HMD.zvec();
									if (Drawparts->tracker_num.size() > 0) {
										v_ = c.mat_WAIST.zvec();
									}
									float x_1 = -sinf(c.body_yrad);
									float y_1 = cosf(c.body_yrad);
									float x_2 = v_.x();
									float y_2 = -v_.z();
									float r_ = std::atan2f(x_1*y_2 - x_2 * y_1, x_1*x_2 + y_1 * y_2);
									c.body_yrad += r_ * FRAME_RATE / GetFPS() / 10.f;
								}
								//�g��,����,��
								MATRIX_ref m_inv = MATRIX_ref::RotY(DX_PI_F + c.body_yrad);
								{
									c.body.SetMatrix(m_inv);
									if (Drawparts->tracker_num.size() > 0) {
										//��
										c.body.SetFrameLocalMatrix(c.frame_.bodyb_f.first, (c.mat_WAIST*m_inv.Inverse())*MATRIX_ref::Mtrans(c.frame_.bodyb_f.second));
										//����
										c.body.SetFrameLocalMatrix(c.frame_.head_f.first, (MATRIX_ref::Axis1(c.mat_HMD.xvec()*-1.f, c.mat_HMD.yvec(), c.mat_HMD.zvec()*-1.f) *m_inv.Inverse()*(c.mat_WAIST*m_inv.Inverse()).Inverse())
											*MATRIX_ref::Mtrans(c.frame_.head_f.second));
									}
									else {
										//����
										c.body.SetFrameLocalMatrix(c.frame_.head_f.first, (MATRIX_ref::Axis1(c.mat_HMD.xvec()*-1.f, c.mat_HMD.yvec(), c.mat_HMD.zvec()*-1.f) *m_inv.Inverse())
											*MATRIX_ref::Mtrans(c.frame_.head_f.second));
									}
									c.body.SetMatrix(m_inv
										*MATRIX_ref::Mtrans((c.pos - c.rec_HMD) + c.pos_HMD - (c.body.frame(c.frame_.RIGHTeye_f.first) + (c.body.frame(c.frame_.LEFTeye_f.first) - c.body.frame(c.frame_.RIGHTeye_f.first))*0.5f)));
								}
								//��
								{
									//��
									if (Drawparts->tracker_num.size() > 1) {
										auto& ptr_ = (*Drawparts->get_device())[Drawparts->tracker_num[1]];
										Drawparts->GetDevicePositionVR(Drawparts->tracker_num[1], &mine.pos_LEFTREG, &mine.mat_LEFTREG);
										c.mat_LEFTREG = MATRIX_ref::Axis1(c.mat_LEFTREG.xvec()*-1.f, c.mat_LEFTREG.yvec(), c.mat_LEFTREG.zvec()*-1.f);
										if (mine.start_c || (ptr_.turn && ptr_.now) != oldv_2) {
											mine.mat_LEFTREG_rep = mine.mat_LEFTREG;
										}
										oldv_2 = ptr_.turn && ptr_.now;
										mine.mat_LEFTREG = 
											MATRIX_ref::RotY(deg2rad(90+60-10))*
											mine.mat_LEFTREG_rep.Inverse()*mine.mat_LEFTREG;
										mine.pos_LEFTREG = mine.pos_LEFTREG + (mine.pos - mine.rec_HMD);
										{
											//�
											VECTOR_ref tgt_pt = c.pos_LEFTREG;
											VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - c.body.frame(c.frame_.LEFTfoot1_f.first)).Norm(), m_inv.Inverse());//�
											VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, 1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0�Ƃ���
											float cos_t = getcos_tri((c.body.frame(c.frame_.LEFTreg_f.first) - c.body.frame(c.frame_.LEFTfoot2_f.first)).size(), (c.body.frame(c.frame_.LEFTfoot2_f.first) - c.body.frame(c.frame_.LEFTfoot1_f.first)).size(), (c.body.frame(c.frame_.LEFTfoot1_f.first) - tgt_pt).size());
											VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
											//��r
											c.body.SetFrameLocalMatrix(c.frame_.LEFTfoot1_f.first, MATRIX_ref::Mtrans(c.frame_.LEFTfoot1_f.second));
											MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_.LEFTfoot2_f.first) - c.body.frame(c.frame_.LEFTfoot1_f.first), m_inv.Inverse()), vec_t);
											c.body.SetFrameLocalMatrix(c.frame_.LEFTfoot1_f.first, a1_inv*MATRIX_ref::Mtrans(c.frame_.LEFTfoot1_f.second));

											//���r
											c.body.SetFrameLocalMatrix(c.frame_.LEFTfoot2_f.first, MATRIX_ref::Mtrans(c.frame_.LEFTfoot2_f.second));
											MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_.LEFTreg_f.first) - c.body.frame(c.frame_.LEFTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - c.body.frame(c.frame_.LEFTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
											c.body.SetFrameLocalMatrix(c.frame_.LEFTfoot2_f.first, a2_inv*MATRIX_ref::Mtrans(c.frame_.LEFTfoot2_f.second));
											//��
											c.body.SetFrameLocalMatrix(c.frame_.LEFTreg_f.first, c.mat_LEFTREG* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(c.frame_.LEFTreg_f.second));
										}
									}
									//�E
									if (Drawparts->tracker_num.size() > 2) {
										auto& ptr_ = (*Drawparts->get_device())[Drawparts->tracker_num[2]];
										Drawparts->GetDevicePositionVR(Drawparts->tracker_num[2], &mine.pos_RIGHTREG, &mine.mat_RIGHTREG);
										c.mat_RIGHTREG = MATRIX_ref::Axis1(c.mat_RIGHTREG.xvec()*-1.f, c.mat_RIGHTREG.yvec(), c.mat_RIGHTREG.zvec()*-1.f);
										if (mine.start_c || (ptr_.turn && ptr_.now) != oldv_3) {
											mine.mat_RIGHTREG_rep = mine.mat_RIGHTREG;
										}
										oldv_3 = ptr_.turn && ptr_.now;
										mine.mat_RIGHTREG = 
											MATRIX_ref::RotY(deg2rad(180-22-10))*
											mine.mat_RIGHTREG_rep.Inverse()*mine.mat_RIGHTREG;
										mine.pos_RIGHTREG = mine.pos_RIGHTREG + (mine.pos - mine.rec_HMD);
										{
											//�
											VECTOR_ref tgt_pt = c.pos_RIGHTREG;
											VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - c.body.frame(c.frame_.RIGHTfoot1_f.first)).Norm(), m_inv.Inverse());//�
											VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, 1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0�Ƃ���
											float cos_t = getcos_tri((c.body.frame(c.frame_.RIGHTreg_f.first) - c.body.frame(c.frame_.RIGHTfoot2_f.first)).size(), (c.body.frame(c.frame_.RIGHTfoot2_f.first) - c.body.frame(c.frame_.RIGHTfoot1_f.first)).size(), (c.body.frame(c.frame_.RIGHTfoot1_f.first) - tgt_pt).size());
											VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
											//��r
											c.body.SetFrameLocalMatrix(c.frame_.RIGHTfoot1_f.first, MATRIX_ref::Mtrans(c.frame_.RIGHTfoot1_f.second));
											MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_.RIGHTfoot2_f.first) - c.body.frame(c.frame_.RIGHTfoot1_f.first), m_inv.Inverse()), vec_t);
											c.body.SetFrameLocalMatrix(c.frame_.RIGHTfoot1_f.first, a1_inv*MATRIX_ref::Mtrans(c.frame_.RIGHTfoot1_f.second));
											//���r
											c.body.SetFrameLocalMatrix(c.frame_.RIGHTfoot2_f.first, MATRIX_ref::Mtrans(c.frame_.RIGHTfoot2_f.second));
											MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_.RIGHTreg_f.first) - c.body.frame(c.frame_.RIGHTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - c.body.frame(c.frame_.RIGHTfoot2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
											c.body.SetFrameLocalMatrix(c.frame_.RIGHTfoot2_f.first, a2_inv*MATRIX_ref::Mtrans(c.frame_.RIGHTfoot2_f.second));
											//��
											c.body.SetFrameLocalMatrix(c.frame_.RIGHTreg_f.first, c.mat_RIGHTREG* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(c.frame_.RIGHTreg_f.second));
										}
									}

								}
								//��
								{
									c.body.get_anime(3).per = 0.f;
									c.body.get_anime(3).time = 0.f;
									//�E��
									{
										Drawparts->GetDevicePositionVR(Drawparts->get_hand1_num(), &c.pos_RIGHTHAND, &c.mat_RIGHTHAND);
										c.pos_RIGHTHAND = c.pos_RIGHTHAND + (c.pos - c.rec_HMD);
										c.mat_RIGHTHAND = MATRIX_ref::Axis1(c.mat_RIGHTHAND.xvec()*-1.f, c.mat_RIGHTHAND.yvec(), c.mat_RIGHTHAND.zvec()*-1.f);
										c.mat_RIGHTHAND = c.mat_RIGHTHAND*MATRIX_ref::RotAxis(c.mat_RIGHTHAND.xvec(), deg2rad(-60));
										c.mat_RIGHTHAND = MATRIX_ref::RotVec2(VGet(0, 0, 1.f), c.vecadd_RIGHTHAND)*c.mat_RIGHTHAND;//���R�C��
										//�e��
										c.obj_gun.SetMatrix(c.mat_RIGHTHAND*MATRIX_ref::Mtrans(c.pos_RIGHTHAND));
										{
											//�
											VECTOR_ref tgt_pt = c.obj_gun.frame(c.ptr_now->frame[5].first);
											VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - c.body.frame(c.frame_.RIGHTarm1_f.first)).Norm(), m_inv.Inverse());//�
											VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0�Ƃ���
											float cos_t = getcos_tri((c.body.frame(c.frame_.RIGHThand_f.first) - c.body.frame(c.frame_.RIGHTarm2_f.first)).size(), (c.body.frame(c.frame_.RIGHTarm2_f.first) - c.body.frame(c.frame_.RIGHTarm1_f.first)).size(), (c.body.frame(c.frame_.RIGHTarm1_f.first) - tgt_pt).size());
											VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
											//��r
											if (Drawparts->tracker_num.size() > 0) {
												c.body.SetFrameLocalMatrix(c.frame_.RIGHTarm1_f.first, (c.mat_WAIST*m_inv.Inverse()).Inverse()*MATRIX_ref::Mtrans(c.frame_.RIGHTarm1_f.second));
											}
											else {
												c.body.SetFrameLocalMatrix(c.frame_.RIGHTarm1_f.first, MATRIX_ref::Mtrans(c.frame_.RIGHTarm1_f.second));
											}
											MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_.RIGHTarm2_f.first) - c.body.frame(c.frame_.RIGHTarm1_f.first), m_inv.Inverse()), vec_t);
											if (Drawparts->tracker_num.size() > 0) {
												c.body.SetFrameLocalMatrix(c.frame_.RIGHTarm1_f.first, a1_inv*(c.mat_WAIST*m_inv.Inverse()).Inverse()*MATRIX_ref::Mtrans(c.frame_.RIGHTarm1_f.second));
											}
											else {
												c.body.SetFrameLocalMatrix(c.frame_.RIGHTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(c.frame_.RIGHTarm1_f.second));
											}
											//���r
											c.body.SetFrameLocalMatrix(c.frame_.RIGHTarm2_f.first, MATRIX_ref::Mtrans(c.frame_.RIGHTarm2_f.second));
											MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_.RIGHThand_f.first) - c.body.frame(c.frame_.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - c.body.frame(c.frame_.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
											c.body.SetFrameLocalMatrix(c.frame_.RIGHTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(c.frame_.RIGHTarm2_f.second));
											//��
											c.body.SetFrameLocalMatrix(c.frame_.RIGHThand_f.first,
												MATRIX_ref::RotY(deg2rad(-10))*
												MATRIX_ref::RotZ(deg2rad(50))*
												MATRIX_ref::RotX(deg2rad(90))*
												c.mat_RIGHTHAND*
												m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(c.frame_.RIGHThand_f.second));
										}
										//�E�l�����w
										c.body.get_anime(0).per = 1.f;
										c.body.get_anime(5).per = c.obj_gun.get_anime(2).per;
									}
									//����
									{
										Drawparts->GetDevicePositionVR(Drawparts->get_hand2_num(), &c.pos_LEFTHAND, &c.mat_LEFTHAND);
										c.pos_LEFTHAND = c.pos_LEFTHAND + (c.pos - c.rec_HMD);
										c.mat_LEFTHAND = MATRIX_ref::Axis1(c.mat_LEFTHAND.xvec()*-1.f, c.mat_LEFTHAND.yvec(), c.mat_LEFTHAND.zvec()*-1.f);
										c.mat_LEFTHAND = c.mat_LEFTHAND*MATRIX_ref::RotAxis(c.mat_LEFTHAND.xvec(), deg2rad(-60));

										{
											float dist_ = (c.pos_LEFTHAND - c.obj_gun.frame(c.ptr_now->frame[3].first)).size();
											if (dist_ <= 0.1f && (!c.reloadf || !c.down_mag)) {
												c.LEFT_hand = true;
												c.pos_LEFTHAND = c.obj_gun.frame(c.ptr_now->frame[3].first);
											}
											else {
												c.LEFT_hand = false;
											}
										}
										{
											//�
											VECTOR_ref tgt_pt = c.pos_LEFTHAND;
											VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - c.body.frame(c.frame_.LEFTarm1_f.first)).Norm(), m_inv.Inverse());//�
											VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0�Ƃ���
											float cos_t = getcos_tri((c.body.frame(c.frame_.LEFThand_f.first) - c.body.frame(c.frame_.LEFTarm2_f.first)).size(), (c.body.frame(c.frame_.LEFTarm2_f.first) - c.body.frame(c.frame_.LEFTarm1_f.first)).size(), (c.body.frame(c.frame_.LEFTarm1_f.first) - tgt_pt).size());
											VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
											//��r
											if (Drawparts->tracker_num.size() > 0) {
												c.body.SetFrameLocalMatrix(c.frame_.LEFTarm1_f.first, (c.mat_WAIST*m_inv.Inverse()).Inverse()*MATRIX_ref::Mtrans(c.frame_.LEFTarm1_f.second));
											}
											else {
												c.body.SetFrameLocalMatrix(c.frame_.LEFTarm1_f.first, MATRIX_ref::Mtrans(c.frame_.LEFTarm1_f.second));
											}
											MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_.LEFTarm2_f.first) - c.body.frame(c.frame_.LEFTarm1_f.first), m_inv.Inverse()), vec_t);
											if (Drawparts->tracker_num.size() > 0) {
												c.body.SetFrameLocalMatrix(c.frame_.LEFTarm1_f.first, a1_inv*(c.mat_WAIST*m_inv.Inverse()).Inverse()*MATRIX_ref::Mtrans(c.frame_.LEFTarm1_f.second));
											}
											else {
												c.body.SetFrameLocalMatrix(c.frame_.LEFTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(c.frame_.LEFTarm1_f.second));
											}

											//���r
											c.body.SetFrameLocalMatrix(c.frame_.LEFTarm2_f.first, MATRIX_ref::Mtrans(c.frame_.LEFTarm2_f.second));
											MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_.LEFThand_f.first) - c.body.frame(c.frame_.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - c.body.frame(c.frame_.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
											c.body.SetFrameLocalMatrix(c.frame_.LEFTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(c.frame_.LEFTarm2_f.second));
											//��
											c.body.SetFrameLocalMatrix(c.frame_.LEFThand_f.first, MATRIX_ref::RotZ(deg2rad(-60))* MATRIX_ref::RotX(deg2rad(80))* c.mat_LEFTHAND* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(c.frame_.LEFThand_f.second));
										}

									}
								}
							}
							else {
								{
									VECTOR_ref v_ = c.mat_HMD.zvec();
									float x_1 = -sinf(c.body_yrad);
									float y_1 = cosf(c.body_yrad);
									float x_2 = v_.x();
									float y_2 = -v_.z();
									float r_ = std::atan2f(x_1*y_2 - x_2 * y_1, x_1*x_2 + y_1 * y_2);
									c.body_yrad += r_ * FRAME_RATE / GetFPS() / 2.f;
								}
								{
									VECTOR_ref v_ = c.mat_HMD.zvec();
									float x_1 = sinf(c.body_xrad);
									float y_1 = -cosf(c.body_xrad);
									float x_2 = -v_.y();
									float y_2 = -std::hypotf(v_.x(), v_.z());
									c.body_xrad += std::atan2f(x_1*y_2 - x_2 * y_1, x_1*x_2 + y_1 * y_2);
								}
								//�g��
								MATRIX_ref mg_inv = MATRIX_ref::RotY(DX_PI_F + c.body_yrad);
								MATRIX_ref mb_inv = MATRIX_ref::RotY(deg2rad(15));
								MATRIX_ref m_inv = MATRIX_ref::RotY(deg2rad(30))*MATRIX_ref::RotZ(c.body_zrad)*MATRIX_ref::RotX(c.body_xrad)*mg_inv;
								{
									if (c.reloadf) {
										mb_inv = MGetIdent();
										m_inv = MATRIX_ref::RotZ(c.body_zrad)*MATRIX_ref::RotX(c.body_xrad)*mg_inv;
									}
									c.body.SetMatrix(MATRIX_ref::Mtrans(c.pos - c.rec_HMD));
									//
									c.body.SetFrameLocalMatrix(c.frame_.bodyg_f.first, mg_inv*MATRIX_ref::Mtrans(c.frame_.bodyg_f.second));
									c.body.SetFrameLocalMatrix(c.frame_.bodyb_f.first, mb_inv*MATRIX_ref::Mtrans(c.frame_.bodyb_f.second));
									c.body.SetFrameLocalMatrix(c.frame_.body_f.first, m_inv*(mb_inv*mg_inv).Inverse()*MATRIX_ref::Mtrans(c.frame_.body_f.second));
								}

								//����
								c.body.SetFrameLocalMatrix(c.frame_.head_f.first, c.mat_HMD*m_inv.Inverse()*MATRIX_ref::Mtrans(c.frame_.head_f.second));
								if (c.reloadf) {
									c.body.frame_reset(c.frame_.head_f.first);
								}
								//��
								{
									auto speed = (c.running ? 6.f : ((c.ads.first ? 2.f : 4.f)*(c.squat.first ? 0.4f : 1.f))) / GetFPS();
									auto ratio_t = c.add_pos.size() / speed;
									if (c.running) {
										easing_set(&c.body.get_anime(8).per, 0.f, 0.95f);
										easing_set(&c.body.get_anime(7).per, 0.f, 0.95f);
										easing_set(&c.body.get_anime(2).per, 1.f*ratio_t, 0.95f);
										easing_set(&c.body.get_anime(1).per, 0.f, 0.95f);
									}
									else if (c.ads.first) {
										easing_set(&c.body.get_anime(2).per, 0.f, 0.95f);
										if (!c.squat.first) {
											easing_set(&c.body.get_anime(1).per, 0.5f*ratio_t, 0.95f);
											easing_set(&c.body.get_anime(8).per, 0.f, 0.9f);
											easing_set(&c.body.get_anime(7).per, 0.f, 0.9f);
										}
										else {
											easing_set(&c.body.get_anime(1).per, 0.f, 0.95f);
											easing_set(&c.body.get_anime(8).per, 0.5f*ratio_t, 0.9f);
											easing_set(&c.body.get_anime(7).per, 1.f - 1.f*ratio_t, 0.9f);
										}
									}
									else {
										easing_set(&c.body.get_anime(2).per, 0.f, 0.95f);
										if (!c.squat.first) {
											easing_set(&c.body.get_anime(1).per, 1.f*ratio_t, 0.95f);
											easing_set(&c.body.get_anime(8).per, 0.f, 0.9f);
											easing_set(&c.body.get_anime(7).per, 0.f, 0.9f);
										}
										else {
											easing_set(&c.body.get_anime(1).per, 0.f, 0.95f);
											easing_set(&c.body.get_anime(8).per, 1.f*ratio_t, 0.9f);
											easing_set(&c.body.get_anime(7).per, 1.f - 1.f*ratio_t, 0.9f);
										}
										easing_set(&c.body.get_anime(0).per, 0.f, 0.95f);
									}
									c.body.get_anime(1).time += 30.f / GetFPS();
									if (c.body.get_anime(1).time >= c.body.get_anime(1).alltime) {
										c.body.get_anime(1).time = 0.f;
									}
									c.body.get_anime(2).time += 30.f / GetFPS();
									if (c.body.get_anime(2).time >= c.body.get_anime(2).alltime) {
										c.body.get_anime(2).time = 0.f;
									}
									c.body.get_anime(8).time += 30.f / GetFPS() * ((c.body.get_anime(8).alltime / 30.f) / c.ptr_now->reload_time);
									if (c.body.get_anime(8).time >= c.body.get_anime(8).alltime) {
										c.body.get_anime(8).time = 0.f;
									}
								}
								//���_
								{
									VECTOR_ref pv = VGet(0, 0, 0);
									if (c.ptr_now->frame[4].first != INT_MAX) {
										pv = c.ptr_now->frame[4].second;
									}
									if (c.ads.first) {
										easing_set(&c.gunpos, VGet(-0.f, 0.f - pv.y() + sin(DX_PI_F*2.f*(c.body.get_anime(1).time / c.body.get_anime(1).alltime))*0.001f*c.body.get_anime(1).per, -0.315f), 0.75f);
										if (&c == &mine) {
											easing_set(&this->campos_TPS, VGet(-0.35f, 0.125f, 1.f), 0.9f);
										}
									}
									else {
										if (c.running) {
											easing_set(&c.gunpos, VGet(-0.1f, -0.1f - pv.y(), -0.25f), 0.9f);
											if (&c == &mine) {
												easing_set(&this->campos_TPS, VGet(-0.35f, 0.125f, 3.f), 0.95f);
											}
										}
										else {
											easing_set(&c.gunpos, VGet(-0.1f, -0.05f - pv.y() + sin(DX_PI_F*2.f*(c.body.get_anime(1).time / c.body.get_anime(1).alltime))*0.002f*c.body.get_anime(1).per, -0.335f), 0.75f);
											if (&c == &mine) {
												easing_set(&this->campos_TPS, VGet(-0.35f, 0.125f, 1.75f), 0.95f);
											}
										}
									}
								}
								//��
								{
									c.body.frame_reset(c.frame_.RIGHTarm1_f.first);
									c.body.frame_reset(c.frame_.RIGHTarm2_f.first);
									c.body.frame_reset(c.frame_.RIGHThand_f.first);
									c.body.frame_reset(c.frame_.LEFTarm1_f.first);
									c.body.frame_reset(c.frame_.LEFTarm2_f.first);
									c.body.frame_reset(c.frame_.LEFThand_f.first);


									if (c.running) {
										c.body.get_anime(6).per = 1.f;
										c.body.get_anime(6).time += 30.f / GetFPS();
										if (c.body.get_anime(6).time >= c.body.get_anime(6).alltime) {
											c.body.get_anime(6).time = 0.f;
										}
									}
									else {
										c.body.get_anime(6).per = 0.f;
										c.body.get_anime(6).time = 0.f;
										if (c.reloadf && c.gun_stat[c.ptr_now->id].mag_in.size() >= 1) {
											c.body.get_anime(3).per = 1.f;
											c.body.get_anime(3).time += 30.f / GetFPS() * ((c.body.get_anime(3).alltime / 30.f) / c.ptr_now->reload_time);
											if (c.body.get_anime(3).time >= c.body.get_anime(3).alltime) {
												c.body.get_anime(3).time = 0.f;
											}
										}
										else {
											c.body.get_anime(3).per = 0.f;
											c.body.get_anime(3).time = 0.f;
											//�E��
											{
												//���_���ꎞ�擾
												c.pos_HMD = (c.body.frame(c.frame_.RIGHTeye_f.first) + (c.body.frame(c.frame_.LEFTeye_f.first) - c.body.frame(c.frame_.RIGHTeye_f.first))*0.5f) -c.pos;
												//�e��
												c.mat_RIGHTHAND = MATRIX_ref::RotVec2(VGet(0, 0, 1.f), c.vecadd_RIGHTHAND)*c.mat_HMD;//���R�C��
												c.pos_RIGHTHAND = c.pos_HMD - c.rec_HMD + MATRIX_ref::Vtrans(c.gunpos, c.mat_RIGHTHAND)+ (c.pos - c.rec_HMD);
												c.obj_gun.SetMatrix(c.mat_RIGHTHAND*MATRIX_ref::Mtrans(c.pos_RIGHTHAND));
												//�
												VECTOR_ref tgt_pt = c.obj_gun.frame(c.ptr_now->frame[5].first);
												VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - c.body.frame(c.frame_.RIGHTarm1_f.first)).Norm(), m_inv.Inverse());
												VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0�Ƃ���
												float cos_t = getcos_tri((c.body.frame(c.frame_.RIGHThand_f.first) - c.body.frame(c.frame_.RIGHTarm2_f.first)).size(), (c.body.frame(c.frame_.RIGHTarm2_f.first) - c.body.frame(c.frame_.RIGHTarm1_f.first)).size(), (c.body.frame(c.frame_.RIGHTarm1_f.first) - tgt_pt).size());
												VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
												//��r
												c.body.SetFrameLocalMatrix(c.frame_.RIGHTarm1_f.first, MATRIX_ref::Mtrans(c.frame_.RIGHTarm1_f.second));
												MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_.RIGHTarm2_f.first) - c.body.frame(c.frame_.RIGHTarm1_f.first), m_inv.Inverse()), vec_t);
												c.body.SetFrameLocalMatrix(c.frame_.RIGHTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(c.frame_.RIGHTarm1_f.second));
												//���r
												c.body.SetFrameLocalMatrix(c.frame_.RIGHTarm2_f.first, MATRIX_ref::Mtrans(c.frame_.RIGHTarm2_f.second));
												MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.frame_.RIGHThand_f.first) - c.body.frame(c.frame_.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - c.body.frame(c.frame_.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
												c.body.SetFrameLocalMatrix(c.frame_.RIGHTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(c.frame_.RIGHTarm2_f.second));
												//��
												c.body.SetFrameLocalMatrix(c.frame_.RIGHThand_f.first, MATRIX_ref::RotY(deg2rad(-10))* MATRIX_ref::RotZ(deg2rad(50))* MATRIX_ref::RotX(deg2rad(90))* c.mat_RIGHTHAND* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(c.frame_.RIGHThand_f.second));
											}
											//����
											{
												if (c.down_mag) {
													c.pos_LEFTHAND = c.obj_gun.frame(c.ptr_now->frame[0].first) + c.mat_RIGHTHAND.yvec()*-0.05f;
												}
												else {
													c.pos_LEFTHAND = c.obj_gun.frame(c.ptr_now->frame[3].first);
												}
												if (!c.reloadf || !c.down_mag) {
													c.LEFT_hand = true;
													c.pos_LEFTHAND = c.obj_gun.frame(c.ptr_now->frame[3].first);
												}
												else {
													c.LEFT_hand = false;
												}
												c.mat_LEFTHAND = c.mat_HMD;

												{
													VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((c.pos_LEFTHAND - c.body.frame(c.frame_.LEFTarm1_f.first)).Norm(), m_inv.Inverse());//�
													VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0�Ƃ���
													float cos_t = getcos_tri((c.body.frame(c.frame_.LEFThand_f.first) - c.body.frame(c.frame_.LEFTarm2_f.first)).size(), (c.body.frame(c.frame_.LEFTarm2_f.first) - c.body.frame(c.frame_.LEFTarm1_f.first)).size(), (c.body.frame(c.frame_.LEFTarm1_f.first) - (c.pos + c.pos_LEFTHAND - (c.pos - c.rec_HMD))).size());
													VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
													//��r
													c.body.SetFrameLocalMatrix(c.frame_.LEFTarm1_f.first, MATRIX_ref::Mtrans(c.frame_.LEFTarm1_f.second));
													MATRIX_ref a1_inv = MATRIX_ref::RotVec2(
														MATRIX_ref::Vtrans(c.body.frame(c.frame_.LEFTarm2_f.first) - c.body.frame(c.frame_.LEFTarm1_f.first), m_inv.Inverse()),
														vec_t
													);
													c.body.SetFrameLocalMatrix(c.frame_.LEFTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(c.frame_.LEFTarm1_f.second));
													//���r
													c.body.SetFrameLocalMatrix(c.frame_.LEFTarm2_f.first, MATRIX_ref::Mtrans(c.frame_.LEFTarm2_f.second));
													MATRIX_ref a2_inv = MATRIX_ref::RotVec2(
														MATRIX_ref::Vtrans(c.body.frame(c.frame_.LEFThand_f.first) - c.body.frame(c.frame_.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()),
														MATRIX_ref::Vtrans(c.pos_LEFTHAND - c.body.frame(c.frame_.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse())
													);
													c.body.SetFrameLocalMatrix(c.frame_.LEFTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(c.frame_.LEFTarm2_f.second));
													//��
													c.body.SetFrameLocalMatrix(c.frame_.LEFThand_f.first,
														MATRIX_ref::RotZ(deg2rad(-60))*
														MATRIX_ref::RotX(deg2rad(80))*
														c.mat_LEFTHAND*
														m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(c.frame_.LEFThand_f.second));
												}
											}
										}
									}
									//�E�l�����w
									{
										c.body.get_anime(0).per = 1.f;
										c.body.get_anime(5).per = c.obj_gun.get_anime(2).per;
									}
								}
							}
							c.body.work_anime();

							//

							c.frame_.copy_frame(c.body, c.colframe_, &c.col);
							c.col.work_anime();
							c.col.RefreshCollInfo(-1);
							//
							if (!(Drawparts->use_vr && (&c == &mine))) {
								//���_�擾
								c.pos_HMD = (c.body.frame(c.frame_.RIGHTeye_f.first) + (c.body.frame(c.frame_.LEFTeye_f.first) - c.body.frame(c.frame_.RIGHTeye_f.first))*0.5f) - c.pos;
								//
								if (c.running) {
									//�e��
									auto mat_T = MATRIX_ref::RotY(deg2rad(45))* MATRIX_ref::RotX(deg2rad(-90))* c.body.GetFrameLocalWorldMatrix(c.frame_.RIGHThand2_f.first);
									c.pos_RIGHTHAND = c.body.frame(c.frame_.RIGHThand_f.first);
									c.obj_gun.SetMatrix(mat_T*MATRIX_ref::Mtrans(c.pos_RIGHTHAND));
									c.obj_gun.SetMatrix(mat_T*MATRIX_ref::Mtrans(c.pos_RIGHTHAND - c.obj_gun.frame(c.ptr_now->frame[5].first) + c.pos_RIGHTHAND));
									//
									c.mat_LEFTHAND = MATRIX_ref::RotY(deg2rad(-90 + 45))* MATRIX_ref::RotX(deg2rad(-90))*  (c.body.GetFrameLocalWorldMatrix(c.frame_.LEFThand2_f.first)*MATRIX_ref::Mtrans(c.body.frame(c.frame_.LEFThand2_f.first)).Inverse());
									c.pos_LEFTHAND = c.body.frame(c.frame_.LEFThand_f.first) + c.mat_LEFTHAND.yvec()*0.1f;
								}
								else {
									if (c.reloadf && c.gun_stat[c.ptr_now->id].mag_in.size() >= 1) {
										//�e��
										auto mat_T = MATRIX_ref::RotY(deg2rad(45))* MATRIX_ref::RotX(deg2rad(-90))* c.body.GetFrameLocalWorldMatrix(c.frame_.RIGHThand2_f.first);
										c.pos_RIGHTHAND = c.body.frame(c.frame_.RIGHThand_f.first);
										c.obj_gun.SetMatrix(mat_T*MATRIX_ref::Mtrans(c.pos_RIGHTHAND));
										c.obj_gun.SetMatrix(mat_T*MATRIX_ref::Mtrans(c.pos_RIGHTHAND - c.obj_gun.frame(c.ptr_now->frame[5].first) + c.pos_RIGHTHAND));
										//
										c.mat_LEFTHAND = MATRIX_ref::RotY(deg2rad(-90 + 45))* MATRIX_ref::RotX(deg2rad(-90))*  (c.body.GetFrameLocalWorldMatrix(c.frame_.LEFThand2_f.first)*MATRIX_ref::Mtrans(c.body.frame(c.frame_.LEFThand2_f.first)).Inverse());
										c.pos_LEFTHAND = c.body.frame(c.frame_.LEFThand_f.first) + c.mat_LEFTHAND.yvec()*0.1f;
									}
									else {
										auto mat_T = MATRIX_ref::RotVec2(VGet(0, 0, 1.f), c.vecadd_RIGHTHAND)*c.mat_HMD;//���R�C��
										c.pos_RIGHTHAND = c.pos_HMD + MATRIX_ref::Vtrans(c.gunpos, mat_T)+ c.pos;
										c.obj_gun.SetMatrix(mat_T*MATRIX_ref::Mtrans(c.pos_RIGHTHAND));
									}
								}
							}
							//�e����
							{
								if (c.obj_gun.get_anime(3).per == 1.f) {
									c.audio.slide.play_3D(c.pos_RIGHTHAND, 1.f);
								}
								c.obj_gun.get_anime(3).per = std::max(c.obj_gun.get_anime(3).per - 12.f / GetFPS(), 0.f);
							}
						}
						//
						for (auto& c : chara) {
							//�ˌ��֘A
							{
								{
									//����
									easing_set(&c.vecadd_RIGHTHAND, c.vecadd_RIGHTHAND_p, 0.75f);
									easing_set(&c.vecadd_RIGHTHAND_p, VGet(0, 0, 1.f), 0.9f);
									//���R�C��
									if (c.gunf) {
										if (c.ammo_cnt >= 1) {
											c.obj_gun.get_anime(0).per = 1.f;
											c.obj_gun.get_anime(1).per = 0.f;
											c.obj_gun.get_anime(0).time += 60.f / GetFPS();
											if (c.obj_gun.get_anime(0).time >= c.obj_gun.get_anime(0).alltime) {
												c.obj_gun.get_anime(0).time = 0.f;
												c.gunf = false;
											}
										}
										else {
											c.obj_gun.get_anime(1).per = 1.f;
											c.obj_gun.get_anime(0).per = 0.f;
											c.obj_gun.get_anime(1).time += 60.f / GetFPS();
											if (c.obj_gun.get_anime(1).time >= c.obj_gun.get_anime(1).alltime) {
												c.obj_gun.get_anime(1).time = c.obj_gun.get_anime(1).alltime;
												c.gunf = false;
											}
										}

									}
									{
										//�}�K�W���r�o
										if (c.obj_gun.get_anime(5).per >= 0.5f && !c.reloadf && c.gun_stat[c.ptr_now->id].mag_in.size() >= 1) {
											c.audio.mag_down.play_3D(c.pos_RIGHTHAND, 1.f);
											int32_t dnm = int32_t(c.ammo_cnt) - 1;
											//�e��
											if (c.ammo_cnt >= 1) {
												c.ammo_cnt = 1;
											}
											else {
												dnm = 0;
											}
											c.gun_stat[c.gun_ptr->id].in -= dnm;
											//�o�C�u���[�V�����@�o�b�e���[������������߃R�����g�A�E�g
											/*
												Drawparts->Haptic(Drawparts->get_hand1_num(), unsigned short(60000));
											*/
											//�}�K�W���r�o
											c.reload_cnt = 0.f;
											c.gun_stat[c.ptr_now->id].mag_in.erase(c.gun_stat[c.ptr_now->id].mag_in.begin());
											//�}�K�W���r�o
											bool tt = false;
											for (auto& g : this->item_data) {
												if (g.ptr == nullptr && g.cate == 1) {
													tt = true;
													g.Set_item(c.ptr_now, c.pos_mag, c.mat_mag, 1);
													g.add = (c.obj_gun.frame(c.ptr_now->frame[1].first) - c.obj_gun.frame(c.ptr_now->frame[0].first)).Norm()*-1.f / GetFPS();//�r䰃x�N�g��
													g.cap = dnm;
													break;
												}
											}
											if (!tt) {
												this->item_data.resize(this->item_data.size() + 1);
												auto& g = this->item_data.back();
												g.Set_item(c.ptr_now, c.pos_mag, c.mat_mag, 1);
												g.add = (c.obj_gun.frame(c.ptr_now->frame[1].first) - c.obj_gun.frame(c.ptr_now->frame[0].first)).Norm()*-1.f / GetFPS();//�r䰃x�N�g��
												g.cap = dnm;
											}
											//
											c.reloadf = true;
										}
										//�Z���N�^�[
										if (c.selkey.second == 1) {
											++c.gun_stat[c.gun_ptr->id].select %= c.ptr_now->select.size();
										}
									}
									//
									if (c.reloadf && c.gun_stat[c.ptr_now->id].mag_in.size() >= 1) {
										c.reload_cnt += 1.f / GetFPS();
										if (Drawparts->use_vr) {
											if (c.reload_cnt < c.ptr_now->reload_time) {
												c.down_mag = false;
											}
										}
									}
									//�Z�t�e�B
									easing_set(&c.obj_gun.get_anime(4).per, float(0.f), 0.5f);
									//�ˌ�
									if (!c.gunf && c.ammo_cnt >= 1) {
										if (c.ptr_now->select[c.gun_stat[c.gun_ptr->id].select] == 2) {//�t���I�[�g�p
											c.trigger.second = 0;
										}
									}
									c.trigger.get_in(c.obj_gun.get_anime(2).per >= 0.5f);
									if (c.trigger.second == 1) {
										c.audio.trigger.play_3D(c.pos_RIGHTHAND, 1.f);
									}

									{
										if (c.trigger.second == 1 && !c.gunf && c.ammo_cnt >= 1) {
											c.gunf = true;
											//�e���Ǘ�
											c.ammo_cnt--;
											c.gun_stat[c.gun_ptr->id].in--;
											if (!c.reloadf && c.gun_stat[c.gun_ptr->id].mag_in.size() >= 1 && c.gun_stat[c.gun_ptr->id].mag_in.front() > 0) {
												c.gun_stat[c.gun_ptr->id].mag_in.front()--;
											}
											//����������ƃR�R�����E�����
											c.vecadd_RIGHTHAND_p = MATRIX_ref::Vtrans(c.vecadd_RIGHTHAND_p,
												MATRIX_ref::RotY(deg2rad(float((int32_t)(c.ptr_now->recoil_xdn*100.f) + GetRand((int32_t)((c.ptr_now->recoil_xup - c.ptr_now->recoil_xdn)*100.f))) / (100.f*(c.LEFT_hand ? 3.f : 1.f))))*
												MATRIX_ref::RotX(deg2rad(float((int32_t)(c.ptr_now->recoil_ydn*100.f) + GetRand((int32_t)((c.ptr_now->recoil_yup - c.ptr_now->recoil_ydn)*100.f))) / (100.f*(c.LEFT_hand ? 3.f : 1.f)))));
											//�e
											c.bullet[c.use_bullet].set(&c.ptr_now->ammo[0], c.obj_gun.frame(c.ptr_now->frame[2].first), c.mat_RIGHTHAND.zvec()*-1.f);
											//�G�t�F�N�g
											c.effcs[ef_fire].set(c.obj_gun.frame(c.ptr_now->frame[2].first), c.mat_RIGHTHAND.zvec()*-1.f, 0.0025f / 0.1f);
											//�T�E���h
											c.audio.shot.play_3D(c.pos_RIGHTHAND, 1.f);
											c.audio.slide.play_3D(c.pos_RIGHTHAND, 1.f);
											//����ID��
											++c.use_bullet %= c.bullet.size();
										}
										//�}�K�W���擾
										if (c.reloadf && c.gun_stat[c.ptr_now->id].mag_in.size() >= 1) {
											if (c.down_mag) {
												if (Drawparts->use_vr) {
													auto p = MATRIX_ref::RotVec2(c.mat_LEFTHAND.yvec(), (c.obj_gun.frame(c.ptr_now->frame[0].first) - c.pos_LEFTHAND));
													c.mat_mag = c.obj_mag.GetFrameLocalMatrix(3)* (c.mat_LEFTHAND*p);
												}
												else {
													c.mat_mag = c.mat_LEFTHAND;
												}
												c.pos_mag = c.pos_LEFTHAND;
												if (Drawparts->use_vr ? ((c.obj_mag.frame(3) - c.obj_gun.frame(c.ptr_now->frame[0].first)).size() <= 0.05f) : (c.reload_cnt > c.ptr_now->reload_time)) {
													c.obj_gun.get_anime(1).time = 0.f;
													c.obj_gun.get_anime(0).per = 1.f;
													c.obj_gun.get_anime(1).per = 0.f;
													if (c.ammo_cnt == 0) {
														c.obj_gun.get_anime(3).per = 1.f;
													}
													c.audio.mag_set.play_3D(c.pos_RIGHTHAND, 1.f);
													if (c.ammo_cnt <= 1) {
														c.ammo_cnt += c.gun_stat[c.gun_ptr->id].mag_in.front();
													}
													c.reloadf = false;
												}
											}
										}
										else {
											c.down_mag = false;
											c.mat_mag = c.mat_RIGHTHAND;
											c.pos_mag = c.obj_gun.frame(c.ptr_now->frame[1].first);
										}
									}
								}
								c.obj_mag.SetMatrix(c.mat_mag* MATRIX_ref::Mtrans(c.pos_mag));
								c.obj_gun.work_anime();
								for (auto& a : c.bullet) {
									if (a.flug) {
										a.repos = a.pos;
										a.pos += a.vec * (a.spec->speed / GetFPS());
										//����
										{
											auto p = mapparts->map_col_line(a.repos, a.pos, 0);
											if (p.HitFlag == TRUE) {
												a.pos = p.HitPosition;
											}
											//*
											for (auto& tgt : chara) {
												auto q = tgt.col.CollCheck_Line(a.repos, a.pos, -1);
												if (q.HitFlag == TRUE) {
													a.pos = q.HitPosition;
													//hit
													c.effcs[ef_reco].set(a.pos, q.Normal, 0.1f / 0.1f);
													//
													a.hit = true;
													a.flug = false;
													break;
												}
											}
											if (p.HitFlag == TRUE && a.flug) {
												a.flug = false;
												c.effcs_gndhit[c.use_effcsgndhit].set(a.pos, p.Normal, 0.025f / 0.1f);
												++c.use_effcsgndhit %= c.effcs_gndhit.size();
											}
											//*/
										}
										//����(3�b�������A�X�s�[�h��0�ȉ��A�ђʂ�0�ȉ�)
										if (a.cnt >= 3.f || a.spec->speed < 0.f || a.spec->pene <= 0.f) {
											a.flug = false;
										}
										//
									}
								}
								//
								for (auto& t : c.effcs) {
									const size_t index = &t - &c.effcs[0];
									if (index != ef_smoke) {
										t.put(Drawparts->get_effHandle((int32_t)(index)));
									}
								}
								for (auto& t : c.effcs_gndhit) {
									t.put(Drawparts->get_effHandle(ef_gndsmoke));
								}
							}
							//�A�C�e���֘A
							{
								//�E��
								c.canget_magitem = false;
								if (&c == &chara[0]) {
									for (auto& g : this->item_data) {
										g.Get_item_1(item_data, mapparts);
									}
								}
								for (auto& g : this->item_data) {
									g.Get_item_2(c, mapparts);
								}
							}
						}
						//�������Z�A�A�j���[�V����
						for (auto& c_ : chara) {
							if (c_.start_c) {
								c_.body.PhysicsResetState();
								c_.start_c = false;
							}
							else {
								c_.body.PhysicsCalculation(1000.f / GetFPS());
							}
						}
						//campos,camvec,camup�̎w��
						{
							if (Drawparts->use_vr) {
								cam_easy.campos = mine.pos + mine.pos_HMD - mine.rec_HMD;
								cam_easy.camvec = cam_easy.campos + mine.mat_HMD.zvec();
								cam_easy.camup = mine.mat_HMD.yvec();
							}
							else {
								cam_easy.campos = mine.pos + mine.pos_HMD - mine.rec_HMD;
								cam_easy.camvec = cam_easy.campos + mine.mat_HMD.zvec()*-1.f;
								cam_easy.camup = mine.mat_HMD.yvec();
							}
							cam_easy.fov = deg2rad(Drawparts->use_vr ? 90 : 45);	//
							cam_easy.near_ = 0.1f;
							cam_easy.far_ = 1000.f;
						}
						Set3DSoundListenerPosAndFrontPosAndUpVec(cam_easy.campos.get(), cam_easy.camvec.get(), cam_easy.camup.get());
						UpdateEffekseer3D();
						//�e�p��
						Drawparts->Ready_Shadow(cam_easy.campos,
							[&] {
							for (auto& mine : this->chara) { mine.Draw_chara(); }
							for (auto& g : this->item_data) { g.Draw_item(); }
						},
							VGet(5.f, 2.5f, 5.f), 
							VGet(5.f, 2.5f, 5.f)
							);
						//VR��ԂɓK�p
						Drawparts->Move_Player();
						//
						for (auto& a : mine.bullet) {
							if (a.hit) {
								a.hit = false;
							}
						}
						//�`��
						{
							{
								Hostpassparts->UI_Screen.SetDraw_Screen();
								{
									UIparts->set_draw(mine, Drawparts->use_vr);
								}
								//sky
								Hostpassparts->SkyScreen.SetDraw_Screen(cam_easy.campos - cam_easy.camvec, VGet(0, 0, 0), cam_easy.camup, cam_easy.fov, 1000.0f, 5000.0f);
								{
									mapparts->sky_draw();
								}
								//��ʑ̐[�x�`��
								Hostpassparts->BUF_draw(draw_by_shadow, cam_easy);
								//�ŏI�`��
								Hostpassparts->MAIN_draw();
							}
							//VR�Ɉڂ�
							Drawparts->draw_VR( [&] {
								SetCameraNearFar(0.01f, 2.f);
								SetUseZBuffer3D(FALSE);												//zbufuse
								SetWriteZBuffer3D(FALSE);											//zbufwrite
								DrawBillboard3D((cam_easy.campos + (cam_easy.camvec - cam_easy.campos).Norm()*1.0f).get(), 0.5f, 0.5f, Drawparts->use_vr ? 1.8f : 1.475f, 0.f, Hostpassparts->MAIN_Screen.get(), TRUE);
								SetUseZBuffer3D(TRUE);												//zbufuse
								SetWriteZBuffer3D(TRUE);											//zbufwrite
								//UI
								SetCameraNearFar(0.01f, 2.f);
								SetUseZBuffer3D(FALSE);												//zbufuse
								SetWriteZBuffer3D(FALSE);											//zbufwrite
								DrawBillboard3D((cam_easy.campos + (cam_easy.camvec - cam_easy.campos).Norm()*1.0f).get(), 0.5f, 0.5f, Drawparts->use_vr ? 1.8f : 1.475f, 0.f, Hostpassparts->UI_Screen.get(), TRUE);
								SetUseZBuffer3D(TRUE);												//zbufuse
								SetWriteZBuffer3D(TRUE);											//zbufwrite
								//UI2
								UIparts->item_draw(this->item_data, cam_easy.campos);
							}, cam_easy);
						}
						//2P�`��
						if (Drawparts->use_vr) {
							auto& ct = chara[1];
							//cam_s.cam
							cam_easy2.campos = ct.pos + ct.pos_HMD - ct.rec_HMD;
							cam_easy2.camvec = cam_easy2.campos + ct.mat_HMD.zvec()*-1.f;
							cam_easy2.camup = ct.mat_HMD.yvec();
							cam_easy2.fov = deg2rad(45);	//
							cam_easy2.near_ = 0.1f;
							cam_easy2.far_ = 1000.f;

							{
								Hostpass2parts->UI_Screen.SetDraw_Screen();
								{
									UIparts->set_draw(mine, false);
								}
								//sky
								Hostpass2parts->SkyScreen.SetDraw_Screen(cam_easy2.campos - cam_easy2.camvec, VGet(0, 0, 0), cam_easy2.camup, cam_easy2.fov, 1000.0f, 5000.0f);
								{
									mapparts->sky_draw();
								}
								//��ʑ̐[�x�`��
								Hostpass2parts->BUF_draw(draw_by_shadow, cam_easy2);
								//�ŏI�`��
								Hostpass2parts->MAIN_draw();
							}

							//Screen2�Ɉڂ�
							outScreen2.SetDraw_Screen(cam_easy2.campos, cam_easy2.camvec, cam_easy2.camup, cam_easy2.fov, cam_easy2.near_, cam_easy2.far_);
							{
								Hostpass2parts->MAIN_Screen.DrawGraph(0, 0, true);
								//UI
								Hostpass2parts->UI_Screen.DrawGraph(0, 0, true);
								//UI2
								UIparts->item_draw(this->item_data, cam_easy2.campos);
							}
						}
						//�f�B�X�v���C�`��
						{
							this->TPS.get_in(CheckHitKey(KEY_INPUT_LCONTROL) != 0);
							//TPS���_
							if (this->TPS.first) {
								DXDraw::cam_info cam_tmp;
								cam_tmp.campos = mine.pos - mine.rec_HMD + mine.pos_HMD + MATRIX_ref::Vtrans(this->campos_TPS, mine.mat_HMD);
								cam_tmp.camvec = mine.pos - mine.rec_HMD + mine.pos_HMD + MATRIX_ref::Vtrans(VGet(-0.35f, 0.125f, 0.f), mine.mat_HMD);
								if (Drawparts->use_vr) {
									MATRIX_ref mat = (mine.obj_gun.GetMatrix()*MATRIX_ref::Mtrans(mine.pos_RIGHTHAND).Inverse());
									cam_tmp.campos = mine.pos - mine.rec_HMD + mine.pos_HMD + MATRIX_ref::Vtrans(this->campos_TPS, mat);
									cam_tmp.camvec = mine.pos - mine.rec_HMD + mine.pos_HMD + MATRIX_ref::Vtrans(VGet(-0.35f, 0.125f, 0.f), mat);
								}
								cam_tmp.camup = VGet(0, 1.f, 0);
								cam_tmp.fov = deg2rad(45);
								cam_tmp.near_ = 0.2f;
								cam_tmp.far_ = 100.f;
								//sky
								Hostpass2parts->SkyScreen.SetDraw_Screen(cam_tmp.campos - cam_tmp.camvec, VGet(0, 0, 0), cam_tmp.camup, cam_tmp.fov, 1000.0f, 5000.0f);
								{
									mapparts->sky_draw();
								}
								//��ʑ̐[�x�`��
								{
									Hostpass2parts->BUF_draw(draw_by_shadow, cam_tmp);
								}
								//�ŏI�`��
								Hostpass2parts->MAIN_draw();
								GraphHandle::SetDraw_Screen((int32_t)DX_SCREEN_BACK, cam_tmp.campos, cam_tmp.camvec, cam_tmp.camup, cam_tmp.fov, cam_tmp.near_, cam_tmp.far_);
								{
									Hostpass2parts->MAIN_Screen.DrawGraph(0, 0, true);
								}
							}
							else {//FPS���_
								GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), false);
								{
									if (Drawparts->use_vr) {
										outScreen2.DrawGraph(0, 0, false);
									}
									else {
										Drawparts->outScreen[0].DrawGraph(0, 0, false);
									}
								}
							}
							//�f�o�b�O
							Debugparts->end_way();
							Debugparts->debug(10, 10, float(GetNowHiPerformanceCount() - waits) / 1000.f);
						}
					}
					//��ʂ̔��f
					Drawparts->Screen_Flip(waits);
					//�I������
					if (CheckHitKey(KEY_INPUT_ESCAPE) != 0) {
						this->ending = false;
						break;
					}
					if (CheckHitKey(KEY_INPUT_O) != 0) {
						break;
					}
					//
					for (auto& c : chara) {
						for (auto& t : c.effcs) {
							t.put_end();
						}
						for (auto& t : c.effcs_gndhit) {
							t.put_end();
						}
					}
					//
				}
			}
			//���
			{
				for (auto& c : chara) {
					c.Delete_chara();
				}
				chara.clear();
				mapparts->Delete_map();
				Drawparts->Delete_Shadow();
			}
			//
		} while (ProcessMessage() == 0 && this->ending);
	}
};