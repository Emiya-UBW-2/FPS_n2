#pragma once
class Sceneclass : Mainclass {
private:
	//TPS操作
	class TPS_parts {
	public:
		switchs key_TPS;
		float xr_cam = 0.f;
		float yr_cam = 0.f;
		int sel_cam = 0;
		//カメラ
		cam_info camera_TPS;
		void Set(float& fov_pc) {
			this->key_TPS.ready(false);
			this->camera_TPS.campos = VGet(0, 1.8f, -10);
			this->camera_TPS.set_cam_info(deg2rad(fov_pc), 0.1f, 200.f);
		}
		void Set_info(std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>& MAPPTs, std::vector<Chara>&chara) {
			if (this->key_TPS.on()) {
				{
					const auto fps_ = GetFPS();
					//cam
					for (int i = 0; i < std::min<size_t>(chara.size(), 10); i++) {
						if (CheckHitKey(KEY_INPUT_1 + i) != 0) {
							this->sel_cam = i;
						}
					}
					//pos
					{
						if (CheckHitKey(KEY_INPUT_LEFT) != 0) {
							this->camera_TPS.campos.x(this->camera_TPS.campos.x() - 10.f / fps_ * cos(this->yr_cam));
							this->camera_TPS.campos.z(this->camera_TPS.campos.z() + 10.f / fps_ * sin(this->yr_cam));
						}
						if (CheckHitKey(KEY_INPUT_RIGHT) != 0) {
							this->camera_TPS.campos.x(this->camera_TPS.campos.x() + 10.f / fps_ * cos(this->yr_cam));
							this->camera_TPS.campos.z(this->camera_TPS.campos.z() - 10.f / fps_ * sin(this->yr_cam));
						}
						if (CheckHitKey(KEY_INPUT_UP) != 0) {
							this->camera_TPS.campos.z(this->camera_TPS.campos.z() + 10.f / fps_ * cos(this->yr_cam));
							this->camera_TPS.campos.x(this->camera_TPS.campos.x() + 10.f / fps_ * sin(this->yr_cam));
						}
						if (CheckHitKey(KEY_INPUT_DOWN) != 0) {
							this->camera_TPS.campos.z(this->camera_TPS.campos.z() - 10.f / fps_ * cos(this->yr_cam));
							this->camera_TPS.campos.x(this->camera_TPS.campos.x() - 10.f / fps_ * sin(this->yr_cam));
						}
						this->camera_TPS.campos.x(std::clamp(this->camera_TPS.campos.x(), MAPPTs->map_col_get().mesh_minpos(0).x(), MAPPTs->map_col_get().mesh_maxpos(0).x()));
						this->camera_TPS.campos.z(std::clamp(this->camera_TPS.campos.z(), MAPPTs->map_col_get().mesh_minpos(0).z(), MAPPTs->map_col_get().mesh_maxpos(0).z()));
					}
					//rad
					{
						VECTOR_ref vec_2 = (chara[this->sel_cam].get_head_frame() - this->camera_TPS.campos).Norm();
						VECTOR_ref vec_z = (this->camera_TPS.camvec - this->camera_TPS.campos).Norm();
						VECTOR_ref vec_x = vec_z.cross(this->camera_TPS.camup);

						this->xr_cam -= deg2rad(int(vec_z.cross(vec_2).dot(vec_x) * 50))*0.1f;
						this->yr_cam -= deg2rad(int(vec_x.dot(vec_2) * 50))*0.1f;
						this->xr_cam = std::clamp(this->xr_cam, deg2rad(-89), deg2rad(89));
					}
					//
					this->camera_TPS.camvec = this->camera_TPS.campos + MATRIX_ref::Vtrans(VGet(0, 0, 1), MATRIX_ref::RotX(this->xr_cam)*MATRIX_ref::RotY(this->yr_cam));
					this->camera_TPS.camup = VGet(0, 1.f, 0);
				}
			}
		}
	};
public:
	class SELECT;
	class MAINLOOP;
	//
	class SELECT {
		//
		bool use_VR = false;
		//sound
		SoundHandle assemble;
		SoundHandle shot_se;
		SoundHandle slide_se;
		SoundHandle trigger_se;
		//sound
		SoundHandle decision;
		SoundHandle cancel;
		SoundHandle cursor;
		//カメラ
		cam_info camera_main;
		float fov_base = DX_PI_F / 2;
		//データ
		switchs up;
		switchs down;
		switchs left;
		switchs right;
		switchs shot;
		switchs Start;
		GUNPARTs* parts_p = nullptr;
		size_t parts_cat = SIZE_MAX;
		size_t port_cat = SIZE_MAX;
		Chara::g_parts* port_ptr = nullptr;
		std::vector<Chara::g_parts*> sight_ptr;
		int sel_p = 0;
		int chang = 0;
		int sel_max = 0;
		int chang_max = 0;
		VECTOR_ref viewparts;
		VECTOR_ref viewparts_buf;
		VECTOR_ref camparts;
		float xrad_t = 30.f;
		float yrad_t = -135.f;
		float range_t = 0.25f;
		float range_tgt = 0.25f;
		switchs Rot;
		std::vector<int> sight_i;
		bool Start_b = true;
		float rate = 1.0f;
		Chara* mine_ptr = nullptr;
		//
		VECTOR_ref Shadow_minpos;
		VECTOR_ref Shadow_maxpos;
		VECTOR_ref Light_vec;
		COLOR_F Light_color;
		//
		float change_per = 1.f;
		//
		std::unique_ptr<UIclass::UI_SELECT, std::default_delete<UIclass::UI_SELECT>> UIparts;
	public:
	private:
		//必須品
		void Set_chang_haveto(std::vector<GUNPARTs>&data, const size_t& pts_cat, const size_t& pot_cat, int& chang_t) {
			mine_ptr->delete_parts(pts_cat);

			chang_max = (int)data.size();
			chang_t = std::clamp(chang_t, 0, chang_max - 1);
			parts_p = &data[chang_t];

			mine_ptr->add_parts(parts_p, pts_cat, port_ptr, pot_cat);

			viewparts_buf = port_ptr->obj.frame(port_ptr->rail_f[pot_cat][0].first);
		}
		//非必須品
		void Set_chang_needs(std::vector<GUNPARTs>&data, const size_t& pts_cat, const size_t& pot_cat, int& chang_t, const size_t& sel) {
			chang_max = (int)data.size() + 1;
			chang_t = std::clamp(chang_t, 0, chang_max - 1);
			if (chang_t == 0) { parts_p = nullptr; }
			else { parts_p = &data[std::max(chang_t - 1, 0)]; }
			viewparts_buf = port_ptr->obj.frame(port_ptr->rail_f[pot_cat][0].first);

			switch (sel) {
			case 0:
			{
				mine_ptr->delete_parts(pts_cat);
				break;
			}
			default:
			{
				if (sight_i.size() < sel) {
					sight_i.resize(sel);
				}
				if (sight_i[sel - 1] > 0) {
					mine_ptr->delete_parts(pts_cat, sight_i[sel - 1] - 1);
					if (sight_i[sel - 1] != 3) {
						for (auto&s : sight_i) {
							if (s > 1) {
								s--;
							}
						}
					}
				}
				break;
			}
			}
			if (chang_t != 0) {
				mine_ptr->add_parts(parts_p, pts_cat, port_ptr, pot_cat);
			}
			if (sel > 0) {
				int tgt = int((chang_t != 0) ? mine_ptr->sight_.size() : 0);
				sight_i[sel - 1] = tgt;
			}
		}
		//非必須品3
		void Set_chang_need3(std::vector<GUNPARTs>&data1, std::vector<GUNPARTs>&data2, const size_t& pts_cat, const size_t& pot_cat, int& chang_t) {
			chang_max = (int)(data1.size() + 1 + data2.size());
			chang_t = std::clamp(chang_t, 0, chang_max - 1);
			if (chang_t == 0) { parts_p = nullptr; }
			else {
				if (chang_t < data1.size() + 1) { parts_p = &data1[std::max(chang_t - 1, 0)]; }
				else { parts_p = &data2[std::max(chang_t - 1, 0) - data1.size()]; }
			}
			if (chang_t != 0) {
				mine_ptr->add_parts(parts_p, pts_cat, port_ptr, pot_cat);
			}
			viewparts_buf = port_ptr->obj.frame(port_ptr->rail_f[pot_cat][0].first);
		}
		//
		bool set_sight_at(Chara::g_parts* ports_ptr, std::vector<GUNPARTs>&data) {
			bool trm = false;
			if (ports_ptr->get_attaching()) {
				for (auto& can : ports_ptr->thisparts->can_attach) {
					for (auto& tgt : data) {
						if (can == tgt.mod.get_name()) { trm = true; }
						if (trm) { break; }
					}
					if (trm) { break; }
				}
			}
			return trm;
		}
	public:
		//
		auto& get_Shadow_minpos() { return Shadow_minpos; }
		auto& get_Shadow_maxpos() { return Shadow_maxpos; }
		auto& get_Light_vec() { return Light_vec; }
		auto& get_Light_color() { return Light_color; }
		auto& get_camera(void) { return camera_main; }
		//
		SELECT(std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& DrawPts) {
			use_VR = DrawPts->use_vr;
			this->assemble = SoundHandle::Load("data/audio/assemble.wav");
			this->decision = SoundHandle::Load("data/audio/shot_2.wav");//
			this->cancel = SoundHandle::Load("data/audio/cancel.wav");
			this->cursor = SoundHandle::Load("data/audio/cursor.wav");
			UIparts = std::make_unique<UIclass::UI_SELECT>();
		}
		void Set(std::unique_ptr<OPTION, std::default_delete<OPTION>>& OPTPTs, std::unique_ptr<MAINLOOP, std::default_delete<MAINLOOP>>& MAINLOOPscene) {
			{
				up.ready(false);
				down.ready(false);
				left.ready(false);
				right.ready(false);
				shot.ready(false);
				Start.ready(false);
				Rot.ready(false);
				parts_p = nullptr;
				parts_cat = SIZE_MAX;
				sel_p = 0;
				chang = 0;
				sel_max = 0;
				chang_max = 0;
				viewparts_buf.clear();
				camparts.clear();
				xrad_t = 30.f;
				yrad_t = -135.f;
				range_t = 0.25f;
				range_tgt = 0.25f;
				sight_i.clear();
				Start_b = true;
				rate = 0.1f;
			}
			{
				mine_ptr = &MAINLOOPscene->get_mine();
			}
			{
				SetCreateSoundTimeStretchRate(1.f / std::clamp(rate, 0.9f, 1.1f));
				shot_se = SoundHandle::Load(mine_ptr->get_audio().shot_path);
				slide_se = SoundHandle::Load(mine_ptr->get_audio().slide_path);
				trigger_se = SoundHandle::Load(mine_ptr->get_audio().trigger_path);
				SetCreateSoundTimeStretchRate(1.f);
			}
			SetUseMaskScreenFlag(FALSE);//←カスタム画面でエフェクトが出なくなるため入れる
			SetMousePoint(deskx / 2, desky / 2);											//
			fov_base = deg2rad(use_VR ? 180 : OPTPTs->Fov);
			camera_main.set_cam_info(fov_base, 0.1f, 200.f);//1P
			//ライティング
			Shadow_maxpos = VGet(1.f, 1.f, 1.f);
			Shadow_minpos = VGet(-1.f, -1.f, -1.f);
			Light_vec = VGet(0.5f, -0.5f, 0.5f);
			Light_color = GetColorF(0.42f, 0.41f, 0.40f, 0.0f);
			//必要パーツつける
			{
				//
				{
					parts_p = &MAINLOOPscene->get_mag_data()[0];
					mine_ptr->add_parts(parts_p, EnumGunParts::PARTS_MAGAZINE, nullptr, 0);
				}
				//grip
				{
					parts_p = &MAINLOOPscene->get_grip_data()[0];
					mine_ptr->add_parts(parts_p, EnumGunParts::PARTS_GRIP, &mine_ptr->base, EnumAttachPoint::GRIP_BASE);
				}
				//uperhandguard
				{
					parts_p = &MAINLOOPscene->get_uperhandguard_data()[0];
					mine_ptr->add_parts(parts_p, EnumGunParts::PARTS_UPER_HGUARD, &mine_ptr->base, EnumAttachPoint::UPER_HANDGUARD);
				}
				//underhandguard
				{
					parts_p = &MAINLOOPscene->get_underhandguard_data()[0];
					mine_ptr->add_parts(parts_p, EnumGunParts::PARTS_UNDER_HGUARD, &mine_ptr->base, EnumAttachPoint::UNDER_HANDGUARD);
				}
			}
		}
		bool UpDate(std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>& MAPPTs, std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& DrawPts, std::unique_ptr<MAINLOOP, std::default_delete<MAINLOOP>>& MAINLOOPscene, key_bind& k_) {
			{
				bool changef = false;
				//演算
				{
					{
						mine_ptr->set_gun(VGet(0, 0, 0), MGetIdent());
						mine_ptr->Set_gun();
						mine_ptr->set_mag();
						mine_ptr->Set_mag();

						up.get_in(mine_ptr->get_wkey());
						down.get_in(mine_ptr->get_skey());
						left.get_in(mine_ptr->get_akey());
						right.get_in(mine_ptr->get_dkey());
						shot.get_in(mine_ptr->get_shoot());
						Start.get_in(k_.key_use_ID[14].get_key(0));
						Rot.get_in(k_.mouse_use_ID[1].get_key(3));
						{
							k_.key_use_ID[0].isalways = true;
							k_.key_use_ID[1].isalways = true;
							k_.key_use_ID[2].isalways = true;
							k_.key_use_ID[3].isalways = true;

							k_.key_use_ID[10].isalways = true;
							k_.key_use_ID[14].isalways = true;

							k_.mouse_use_ID[0].isalways = true;
							k_.mouse_use_ID[1].isalways = true;
						}
						if (Start_b) {
							Start_b = false;
							changef = true;
							int pp = mine_ptr->base.thisparts->Select_Chose(1);
							if (pp != -1) {
								mine_ptr->gun_stat_now->get_select() = uint8_t(pp);
							}
						}
						if (left.push()) {
							sel_p--;
							chang = 0;
							changef = true;
						}
						if (right.push()) {
							sel_p++;
							chang = 0;
							changef = true;
						}
						if (sel_p < 0) { sel_p = sel_max - 1; }
						if (sel_p > sel_max - 1) { sel_p = 0; }

						if (up.push()) {
							chang--;
							changef = true;
						}
						if (down.push()) {
							chang++;
							changef = true;
						}
						if (chang < 0) { chang = chang_max - 1; }
						if (chang > chang_max - 1) { chang = 0; }
					}
					{
						sight_ptr.resize(5);
					}
					if (changef) {
						Rot.first = false;
						port_ptr = nullptr;
						sel_max = 0;
						//grip
						{
							if (sel_p == sel_max) {
								port_ptr = &mine_ptr->base;
								port_cat = EnumAttachPoint::GRIP_BASE;
								parts_cat = EnumGunParts::PARTS_GRIP;
								Set_chang_haveto(MAINLOOPscene->get_grip_data(), parts_cat, port_cat, chang);
							}
							sel_max++;
						}
						//uperhandguard
						{
							if (sel_p == sel_max) {
								port_ptr = &mine_ptr->base;
								port_cat = EnumAttachPoint::UPER_HANDGUARD;
								parts_cat = EnumGunParts::PARTS_UPER_HGUARD;
								Set_chang_haveto(MAINLOOPscene->get_uperhandguard_data(), parts_cat, port_cat, chang);
								//
								if (set_sight_at(&mine_ptr->uperhandguard, MAINLOOPscene->get_sight_data())) {
									sight_ptr[0] = &mine_ptr->uperhandguard;
								}
								//
							}
							sel_max++;
						}
						//underhandguard
						{
							if (sel_p == sel_max) {
								port_ptr = &mine_ptr->base;
								port_cat = EnumAttachPoint::UNDER_HANDGUARD;
								parts_cat = EnumGunParts::PARTS_UNDER_HGUARD;
								Set_chang_haveto(MAINLOOPscene->get_underhandguard_data(), parts_cat, port_cat, chang);
							}
							sel_max++;
						}
						//foregrip
						{
							port_ptr = &mine_ptr->underhandguard;
							port_cat = EnumAttachPoint::UNDER_RAIL;
							if (port_ptr->rail_f[port_cat][0].first > 0) {
								if (sel_p == sel_max) {
									parts_cat = EnumGunParts::PARTS_FOREGRIP;
									Set_chang_needs(MAINLOOPscene->get_foregrip_data(), parts_cat, port_cat, chang, 0);
								}
								sel_max++;
							}
						}
						//light/laser
						{
							port_ptr = &mine_ptr->underhandguard;
							port_cat = EnumAttachPoint::LEFTSIDE_RAIL;
							if (port_ptr->rail_f[port_cat][0].first > 0) {
								if (sel_p == sel_max) {
									if (chang == 0) {
										parts_cat = EnumGunParts::PARTS_LIGHT;
									}
									else {
										parts_cat = (chang < MAINLOOPscene->get_light_data().size() + 1) ? EnumGunParts::PARTS_LIGHT : EnumGunParts::PARTS_LASER;
									}
									mine_ptr->delete_parts(EnumGunParts::PARTS_LIGHT);
									mine_ptr->delete_parts(EnumGunParts::PARTS_LASER);
									Set_chang_need3(MAINLOOPscene->get_light_data(), MAINLOOPscene->get_laser_data(), parts_cat, port_cat, chang);
								}
								sel_max++;
							}
						}
						//sidemount
						{
							if (sel_p == sel_max) {
								port_ptr = &mine_ptr->base;
								port_cat = EnumAttachPoint::SIDEMOUNT_BASE;
								parts_cat = EnumGunParts::PARTS_MOUNT;
								{
									chang_max = (int)1 + 1; //chang_max = (int)mount_data.size();
									chang = std::clamp(chang, 0, chang_max - 1);
									if (chang == 0) { parts_p = nullptr; }
									else { parts_p = &MAINLOOPscene->get_mount_data()[std::max(chang - 1, 0)]; }
									mine_ptr->delete_parts(parts_cat, 0);
									if (chang != 0) {
										mine_ptr->add_parts(parts_p, parts_cat, port_ptr, port_cat);
									}
									viewparts_buf = port_ptr->obj.frame(port_ptr->rail_f[port_cat][0].first);
								}
							}
							sel_max++;
						}
						//sidemount_2
						{
							if (mine_ptr->mount_.size() > 0) {
								port_ptr = &mine_ptr->mount_[0];
								port_cat = EnumAttachPoint::SIDEMOUNT;
								if (port_ptr->rail_f[port_cat][0].first > 0) {
									if (sel_p == sel_max) {
										parts_cat = EnumGunParts::PARTS_MOUNT;
										{
											chang_max = (int)1 + 1;
											chang = std::clamp(chang, 0, chang_max - 1);
											if (chang == 0) { parts_p = nullptr; }
											else { parts_p = &MAINLOOPscene->get_mount_data()[chang]; }
											if (mine_ptr->mount_.size() == 2) {
												mine_ptr->delete_parts(parts_cat, 1);
											}
											if (chang != 0) {
												mine_ptr->add_parts(parts_p, parts_cat, port_ptr, port_cat);
											}
											viewparts_buf = port_ptr->obj.frame(port_ptr->rail_f[port_cat][0].first);
										}
										//
										if (mine_ptr->mount_.size() > 1) {
											if (set_sight_at(&mine_ptr->mount_[1], MAINLOOPscene->get_sight_data())) {
												sight_ptr[1] = &mine_ptr->mount_[1];
											}
										}
										//
									}
									sel_max++;
								}
							}
						}
						//マズル
						{
							if (sel_p == sel_max) {
								port_ptr = &mine_ptr->base;
								port_cat = EnumAttachPoint::MAZZULE_BASE;
								parts_cat = EnumGunParts::PARTS_MAZZULE;
								Set_chang_needs(MAINLOOPscene->get_mazzule_data(), parts_cat, port_cat, chang, 0);
							}
							sel_max++;
						}
						//ダストカバー
						{
							if (sel_p == sel_max) {
								port_ptr = &mine_ptr->base;
								port_cat = EnumAttachPoint::DUSTCOVER_BASE;
								parts_cat = EnumGunParts::PARTS_DUSTCOVER;
								Set_chang_needs(MAINLOOPscene->get_dustcover_data(), parts_cat, port_cat, chang, 0);
								//
								if (set_sight_at(&mine_ptr->dustcover, MAINLOOPscene->get_sight_data())) {
									sight_ptr[2] = &mine_ptr->dustcover;
								}
								//
							}
							sel_max++;
						}
						//ストック
						{
							if (sel_p == sel_max) {
								port_ptr = &mine_ptr->base;
								port_cat = EnumAttachPoint::STOCK_BASE;
								parts_cat = EnumGunParts::PARTS_STOCK;
								Set_chang_needs(MAINLOOPscene->get_stock_data(), parts_cat, port_cat, chang, 0);
							}
							sel_max++;
						}
						//sight
						{
							int ssp = sel_max;
							for (auto& s : sight_ptr) {
								if (s != nullptr) {
									port_cat = EnumAttachPoint::UPER_RAIL;
									if (s->rail_f[port_cat][0].first > 0) {
										if (sel_p == sel_max) {
											port_ptr = s;
											parts_cat = EnumGunParts::PARTS_SIGHT;
											Set_chang_needs(MAINLOOPscene->get_sight_data(), parts_cat, port_cat, chang, 1 + sel_max - ssp);
										}
										sel_max++;
									}
								}
							}
							//
						}
						//
						{
							change_per = 1.f;
							assemble.play(DX_PLAYTYPE_BACK, TRUE);

							xrad_t = viewparts_buf.y()*1000.f;
							yrad_t = ((viewparts_buf.x() > 0.f) ? 90.f - viewparts_buf.z()*100.f : -90.f + viewparts_buf.z()*100.f);
						}
						//
					}
					//
					easing_set(&mine_ptr->get_gunanime_trigger()->per, shot.press(), 0.5f);
					if (shot.push()) {
						if (!mine_ptr->get_gunf()) {
							mine_ptr->get_gunf() = true;
							//todo ディレイつける
							{
								shot_se.play(DX_PLAYTYPE_BACK, TRUE);
								slide_se.play(DX_PLAYTYPE_BACK, TRUE);
								trigger_se.play(DX_PLAYTYPE_BACK, TRUE);
								//薬莢
								mine_ptr->calc_cart(rate);
							}
							//エフェクト
							mine_ptr->calc_shot_effect();
						}
					}
					//
					mine_ptr->set_select();
					//
					mine_ptr->set_shot(rate);
					//
					mine_ptr->base.obj.work_anime();

					//薬莢の処理
					mine_ptr->update_cart(MAPPTs);
					mine_ptr->update_effect(DrawPts);
				}
				//campos,camvec,camupの指定
				{}
				{
					{
						int x_m, y_m;
						GetMousePoint(&x_m, &y_m);
						x_m -= deskx / 2;
						y_m -= desky / 2;
						SetMousePoint(deskx / 2, desky / 2);
						SetMouseDispFlag(FALSE);
						xrad_t = std::clamp(xrad_t + float(std::clamp(y_m, -60, 60))*0.1f, -80.f, 80.f);
						yrad_t += float(std::clamp(x_m, -60, 60))*0.1f;
						yrad_t = rad2deg(atan2f(sin(deg2rad(yrad_t)), cos(deg2rad(yrad_t))));
					}
					{
						if (Rot.on()) {
							viewparts.clear();
						}
						else {
							viewparts = viewparts_buf;
						}
					}
					{
						auto tmp = viewparts;
						if (std::abs(yrad_t) > 90) {
							if (mine_ptr->mazzule.get_mazzuletype() > 0) {
								tmp = VECTOR_ref(mine_ptr->mazzule.mazzule_pos()) - tmp;
							}
							else {
								tmp = mine_ptr->base.obj.frame(mine_ptr->base.thisparts->frame_s.mazzule_f.first) - tmp;
							}
						}
						else {
							tmp = VECTOR_ref(VGet(0, 0, 0.5f)) - tmp;//todo
						}
						range_tgt = std::hypotf(sin(deg2rad(yrad_t))*0.25f, cos(deg2rad(yrad_t))*(std::abs(tmp.z()) + camera_main.near_ * 2.f));
					}
					range_t = (Rot.on() && !changef) ? std::clamp(range_t - float(GetMouseWheelRotVol())*0.1f, range_tgt, 5.f) : range_tgt;

					easing_set(&camparts, VGet(
						range_t*cos(deg2rad(xrad_t))*sin(deg2rad(yrad_t)),
						range_t*sin(deg2rad(xrad_t)),
						range_t*cos(deg2rad(xrad_t))*cos(deg2rad(yrad_t))),
						0.8f
					);
					camera_main.camvec = viewparts;
					camera_main.campos = camera_main.camvec + camparts;
					camera_main.camup = VGet(0, 1.f, 0);
				}
			}
			//終了判定
			if (k_.key_use_ID[11].get_key(0)) {
				return false;
			}
			if (Start.push()) {
				return false;
			}
			return true;
			//
		}
		void Dispose(void) {
			mine_ptr->gun_stat_now->get_select() = 0;
			shot_se.Dispose();
			slide_se.Dispose();
			trigger_se.Dispose();
		}
		void UI_Draw(std::unique_ptr<MAINLOOP, std::default_delete<MAINLOOP>>& MAINLOOPscene) {
			UIparts->Set_Draw(MAINLOOPscene, parts_cat, Rot, mine_ptr, parts_p, change_per, use_VR);
			easing_set(&change_per, 0.f, 0.5f);
		}
		void BG_Draw(void) {
			DrawBox(0, 0, 1920, 1080, GetColor(192, 192, 192), TRUE);
		}
		void Shadow_Draw_Far() {

		}
		void Shadow_Draw(void) {
			mine_ptr->Draw_gun();
			mine_ptr->magazine.Draw();
		}
		void Main_Draw(void) {
			mine_ptr->Draw_gun();
			mine_ptr->magazine.Draw();
		}
	};
	//
	class MAINLOOP {
		//カメラ
		cam_info camera_main;
		float fov_base = DX_PI_F / 2;
		bool use_VR = false;
		//データ
		MV1 body_obj, body_col;						//身体モデル
		MV1 hit_pic;								//弾痕
		GraphHandle light;							//ライトテクスチャ
		std::vector<GUNPARTs> mazzule_data;			//GUNデータ
		std::vector<GUNPARTs> grip_data;			//GUNデータ
		std::vector<GUNPARTs> uperhandguard_data;	//GUNデータ
		std::vector<GUNPARTs> underhandguard_data;	//GUNデータ
		std::vector<GUNPARTs> stock_data;			//GUNデータ
		std::vector<GUNPARTs> dustcover_data;		//GUNデータ
		std::vector<GUNPARTs> sight_data;			//GUNデータ
		std::vector<GUNPARTs> foregrip_data;		//GUNデータ
		std::vector<GUNPARTs> light_data;			//GUNデータ
		std::vector<GUNPARTs> laser_data;			//GUNデータ
		std::vector<GUNPARTs> mount_data;			//GUNデータ
		std::vector<GUNPARTs> gun_data;				//GUNデータ
		std::vector<GUNPARTs> mag_data;				//GUNデータ
		std::vector<Meds> meds_data;				//GUNデータ
		//オブジェ
		std::vector<Chara> chara;					//キャラ
		std::vector<Items> item;					//アイテム
		std::vector<Hit> hit_obj;					//弾痕
		size_t hit_buf = 0;							//
		//
		std::unique_ptr<HostPassEffect, std::default_delete<HostPassEffect>> Hostpassparts_TPS;
		std::unique_ptr<TPS_parts, std::default_delete<TPS_parts>> TPSparts;
		std::unique_ptr<UIclass::UI_MAINLOOP, std::default_delete<UIclass::UI_MAINLOOP>> UIparts;
		std::unique_ptr<RULE_parts, std::default_delete<RULE_parts>> RULEparts;
		std::unique_ptr<MAPclass::MiniMap, std::default_delete<MAPclass::MiniMap>> minimapparts;
		//
		VECTOR_ref Shadow_minpos;
		VECTOR_ref Shadow_maxpos;
		VECTOR_ref Light_vec;
		COLOR_F Light_color;
	public:
		//
		auto& get_Shadow_minpos() { return Shadow_minpos; }
		auto& get_Shadow_maxpos() { return Shadow_maxpos; }
		auto& get_Light_vec() { return Light_vec; }
		auto& get_Light_color() { return Light_color; }
		std::vector<Chara>& get_chara(void) { return this->chara; }
		std::vector<Items>& get_items(void) { return this->item; }
		Chara& get_mine(void) { return this->chara[0]; }
		std::vector<GUNPARTs>& get_mazzule_data(void) { return this->mazzule_data; }
		std::vector<GUNPARTs>& get_grip_data(void) { return this->grip_data; }
		std::vector<GUNPARTs>& get_uperhandguard_data(void) { return this->uperhandguard_data; }
		std::vector<GUNPARTs>& get_underhandguard_data(void) { return this->underhandguard_data; }
		std::vector<GUNPARTs>& get_stock_data(void) { return this->stock_data; }
		std::vector<GUNPARTs>& get_dustcover_data(void) { return this->dustcover_data; }
		std::vector<GUNPARTs>& get_sight_data(void) { return this->sight_data; }
		std::vector<GUNPARTs>& get_foregrip_data(void) { return this->foregrip_data; }
		std::vector<GUNPARTs>& get_light_data(void) { return this->light_data; }
		std::vector<GUNPARTs>& get_laser_data(void) { return this->laser_data; }
		std::vector<GUNPARTs>& get_mount_data(void) { return this->mount_data; }
		std::vector<GUNPARTs>& get_gun_data(void) { return this->gun_data; }
		std::vector<GUNPARTs>& get_mag_data(void) { return this->mag_data; }
		std::vector<Meds>& get_meds_data(void) { return this->meds_data; }
		auto& get_camera(void) { return this->camera_main; }
		//
		MAINLOOP(std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& DrawPts, std::unique_ptr<OPTION, std::default_delete<OPTION>>& OPTPTs) {
			//
			use_VR = DrawPts->use_vr;
			//
			UIparts = std::make_unique<UIclass::UI_MAINLOOP>();
			TPSparts = std::make_unique<TPS_parts>();
			RULEparts = std::make_unique<RULE_parts>();
			Hostpassparts_TPS = std::make_unique<HostPassEffect>(OPTPTs->DoF, OPTPTs->Bloom, OPTPTs->SSAO, deskx, desky);	//ホストパスエフェクト(フルスクリーン向け、TPS用)
			minimapparts = std::make_unique<MAPclass::MiniMap>();															//ミニマップ
			{
				//model
				light = GraphHandle::Load("data/light.png");					//ライト
				MV1::Load("data/model/body2/model.mv1", &this->body_obj, true);	//身体
				MV1::Load("data/model/body2/col.mv1", &this->body_col, true);	//身体col
				MV1::Load("data/model/hit/model.mv1", &this->hit_pic, true);	//弾痕
				//
				this->mazzule_data.resize(2);
				this->mazzule_data[0].mod.Ready("data/parts/mazzule/", "00_AK_6P1_Compensator");
				this->mazzule_data[1].mod.Ready("data/parts/mazzule/", "01_sup");
				this->grip_data.resize(3);
				this->grip_data[0].mod.Ready("data/parts/grip/", "00_AK_6P1_Wood_Grip");
				this->grip_data[1].mod.Ready("data/parts/grip/", "01_AK_original_Grip");
				this->grip_data[2].mod.Ready("data/parts/grip/", "02_AK_Zenit_RK3_Grip");
				this->uperhandguard_data.resize(4);
				this->uperhandguard_data[0].mod.Ready("data/parts/uper_handguard/", "00_AK_6P1_Uper_Handguard");
				this->uperhandguard_data[1].mod.Ready("data/parts/uper_handguard/", "01_AK_Original_Polymer_Uper_Handguard");
				this->uperhandguard_data[2].mod.Ready("data/parts/uper_handguard/", "02_AK_Steel_Uper_Handguard");
				this->uperhandguard_data[3].mod.Ready("data/parts/uper_handguard/", "03_AK_Rail_Band");
				this->underhandguard_data.resize(3);
				this->underhandguard_data[0].mod.Ready("data/parts/under_handguard/", "00_AK_6P1_Under_Handguard");
				this->underhandguard_data[1].mod.Ready("data/parts/under_handguard/", "01_AK_Original_Polymer_Under_Handguard");
				this->underhandguard_data[2].mod.Ready("data/parts/under_handguard/", "02_AK_Zenit_B10M_Under_Handguard");
				this->mount_data.resize(2);
				this->mount_data[0].mod.Ready("data/parts/mount/", "00_AK_Dovetail_Mount");
				this->mount_data[1].mod.Ready("data/parts/mount/", "01_AK_SideRailMount");
				this->stock_data.resize(1);
				this->stock_data[0].mod.Ready("data/parts/stock/", "00_AK_6P1_Stock");
				this->dustcover_data.resize(2);
				this->dustcover_data[0].mod.Ready("data/parts/dustcover/", "00_AK_6P1_Dustcover");
				this->dustcover_data[1].mod.Ready("data/parts/dustcover/", "01_AK_DogLegRail_Gen3_dustcover");
				this->sight_data.resize(2);
				this->sight_data[0].mod.Ready("data/parts/sight/", "00_Eotech_XPS3_HoloSight");
				this->sight_data[1].mod.Ready("data/parts/sight/", "01_AIMPOINT_T1_DotSight");
				this->foregrip_data.resize(2);
				this->foregrip_data[0].mod.Ready("data/parts/foregrip/", "00_Standard_Vertical_Foregrip");
				this->foregrip_data[1].mod.Ready("data/parts/foregrip/", "01_Bipod_Vertical_Foregrip");
				this->light_data.resize(1);
				this->light_data[0].mod.Ready("data/parts/light/", "00_Tactical_Flashlight");
				this->laser_data.resize(1);
				this->laser_data[0].mod.Ready("data/parts/laser/", "00_NcSTAR_Laser");
				this->gun_data.resize(1);
				this->gun_data[0].mod.Ready("data/gun/", "00_AKM");
				//MAGデータ
				this->mag_data.resize(1);
				this->mag_data[0].mod.Ready("data/mag/", "00_AK_55_Magazine");
				//MEDデータ
				this->meds_data.resize(1);
				this->meds_data[0].mod.Ready("data/medkit/", "AIDkit");
			}
		}
		void first_after() {
			//GUNデータ2
			for (auto& g : this->mazzule_data) { g.Set_datas(&g - &this->mazzule_data[0], EnumGunParts::PARTS_MAZZULE); }
			for (auto& g : this->grip_data) { g.Set_datas(&g - &this->grip_data[0], EnumGunParts::PARTS_NONE); }
			for (auto& g : this->uperhandguard_data) { g.Set_datas(&g - &this->uperhandguard_data[0], EnumGunParts::PARTS_NONE); }
			for (auto& g : this->underhandguard_data) { g.Set_datas(&g - &this->underhandguard_data[0], EnumGunParts::PARTS_NONE); }
			for (auto& g : this->mount_data) { g.Set_datas(&g - &this->mount_data[0], EnumGunParts::PARTS_NONE); }
			for (auto& g : this->stock_data) { g.Set_datas(&g - &this->stock_data[0], EnumGunParts::PARTS_STOCK); }
			for (auto& g : this->dustcover_data) { g.Set_datas(&g - &this->dustcover_data[0], EnumGunParts::PARTS_NONE); }
			for (auto& g : this->light_data) { g.Set_datas(&g - &this->light_data[0], EnumGunParts::PARTS_NONE); }
			for (auto& g : this->laser_data) { g.Set_datas(&g - &this->laser_data[0], EnumGunParts::PARTS_NONE); }
			for (auto& g : this->foregrip_data) { g.Set_datas(&g - &this->foregrip_data[0], EnumGunParts::PARTS_NONE); }
			for (auto& g : this->sight_data) { g.Set_datas(&g - &this->sight_data[0], EnumGunParts::PARTS_SIGHT); }
			for (auto& g : this->mag_data) { g.Set_datas(&g - &this->mag_data[0], EnumGunParts::PARTS_MAGAZINE); }
			for (auto& g : this->gun_data) { g.Set_datas(&g - &this->gun_data[0], EnumGunParts::PARTS_BASE); }
			//MEDデータ2
			for (auto& g : this->meds_data) { g.Set_datas(&g - &this->meds_data[0]); }
			//弾痕
			this->hit_buf = 0;
			this->hit_obj.resize(64);
			for (auto& h : this->hit_obj) { h.Set(this->hit_pic); }
		}
		void set_Charaa(const size_t &spawn_total) {
			//キャラ設定
			this->chara.resize(spawn_total);
			for (auto& c : this->chara) {
				c.Set(gun_data, 0, body_obj, body_col);
			}
		}
		void Set(std::unique_ptr<OPTION, std::default_delete<OPTION>>& OPTPTs, std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>& MAPPTs) {
			for (auto& c : this->chara) {
				//カスタムattach
				if (&c != &get_mine()) {
					//magazine
					c.add_parts(&mag_data[0], EnumGunParts::PARTS_MAGAZINE, nullptr, 0);
					//grip
					c.add_parts(&grip_data[0], EnumGunParts::PARTS_GRIP, &c.base, EnumAttachPoint::GRIP_BASE);
					//uperhandguard
					c.add_parts(&uperhandguard_data[0], EnumGunParts::PARTS_UPER_HGUARD, &c.base, EnumAttachPoint::UPER_HANDGUARD);
					{
						c.add_parts(&sight_data[0], EnumGunParts::PARTS_SIGHT, &c.uperhandguard, EnumAttachPoint::UPER_RAIL);
					}
					//underhandguard
					c.add_parts(&underhandguard_data[0], EnumGunParts::PARTS_UNDER_HGUARD, &c.base, EnumAttachPoint::UNDER_HANDGUARD);
					{
						c.add_parts(&foregrip_data[0], EnumGunParts::PARTS_FOREGRIP, &c.underhandguard, EnumAttachPoint::UNDER_RAIL);
						c.add_parts(&light_data[0], EnumGunParts::PARTS_LIGHT, &c.underhandguard, EnumAttachPoint::LEFTSIDE_RAIL);
						c.add_parts(&laser_data[0], EnumGunParts::PARTS_LASER, &c.underhandguard, EnumAttachPoint::RIGHTSIDE_RAIL);
					}
					//sidemount
					c.add_parts(&mount_data[0], EnumGunParts::PARTS_MOUNT, &c.base, EnumAttachPoint::SIDEMOUNT_BASE);
					{
						c.add_parts(&mount_data[1], EnumGunParts::PARTS_MOUNT, &c.mount_[0], EnumAttachPoint::SIDEMOUNT);
						{
							c.add_parts(&sight_data[1], EnumGunParts::PARTS_SIGHT, &c.mount_[1], EnumAttachPoint::UPER_RAIL);
						}
					}
					//optional
					c.add_parts(&mazzule_data[0], EnumGunParts::PARTS_MAZZULE, &c.base, EnumAttachPoint::MAZZULE_BASE);
					{
						c.add_parts(&dustcover_data[0], EnumGunParts::PARTS_DUSTCOVER, &c.base, EnumAttachPoint::DUSTCOVER_BASE);
						{
							c.add_parts(&stock_data[0], EnumGunParts::PARTS_STOCK, &c.base, EnumAttachPoint::STOCK_BASE);
						}
					}
					//
				}
				//spawn
				{
					auto&wp = MAPPTs->get_spawn_point()[&c - &this->chara[0]];
					c.spawn(wp, MATRIX_ref::RotY(atan2f(wp.x(), wp.z())));
				}
			}
			//ライティング
			Shadow_maxpos = MAPPTs->map_col_get().mesh_maxpos(0);
			Shadow_minpos = MAPPTs->map_col_get().mesh_minpos(0);
			Light_vec = VGet(0.5f, -0.5f, 0.5f);
			Light_color = GetColorF(0.42f, 0.41f, 0.40f, 0.0f);
			//
			MAPPTs->Start_map();															//環境
			std::for_each(this->chara.begin(), this->chara.end(), [&](Chara& c) { c.Start(MAPPTs); });//プレイヤー操作変数群
			SetMousePoint(deskx / 2, desky / 2);											//
			fov_base = deg2rad(use_VR ? 120 : OPTPTs->Fov);
			camera_main.set_cam_info(fov_base, 0.1f, 200.f);//1P
			TPSparts->Set(OPTPTs->Fov);															//TPS
			RULEparts->Set();																//ルール
		}
		bool UpDate(std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>& MAPPTs, std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& DrawPts, key_bind& k_) {
			{
				//演算
				{
					//座標取得
					get_mine().set_waist(DrawPts);
					//共通
					for (auto& c : this->chara) {
						c.Set_alive(this->item, MAPPTs);
					}
					//プレイヤー操作
					for (auto& c : this->chara) {
						if (&c == &get_mine()) {
							//mine
							if (use_VR) {
								c.operation_VR(DrawPts, !RULEparts->getStart() || RULEparts->getend());
							}
							else {
								c.operation(!RULEparts->getStart() || RULEparts->getend(), camera_main.fov / fov_base);
								{
									k_.key_use_ID[0].isalways = true;
									k_.key_use_ID[1].isalways = true;
									k_.key_use_ID[2].isalways = true;
									k_.key_use_ID[3].isalways = true;
									k_.key_use_ID[4].isalways = true;
									k_.key_use_ID[5].isalways = true;
									k_.key_use_ID[6].isalways = true;
									k_.key_use_ID[7].isalways = true;
									k_.key_use_ID[8].isalways = true;
									k_.key_use_ID[9].isalways = true;
									k_.key_use_ID[10].isalways = true;
									k_.key_use_ID[11].isalways = true;
									k_.key_use_ID[12].isalways = true;
									k_.key_use_ID[13].isalways = true;
									k_.key_use_ID[14].isalways = true;
									k_.key_use_ID[15].isalways = true;
									k_.key_use_ID[16].isalways = true;

									k_.mouse_use_ID[0].isalways = true;
									k_.mouse_use_ID[1].isalways = true;
									k_.mouse_use_ID[2].isalways = true;
								}
							}
						}
						else {
							//cpu
							c.operation_NPC(MAPPTs, this->chara, !RULEparts->getStart() || RULEparts->getend());
						}
						//common
						c.operation_2();
					}
					//
					for (auto& c : this->chara) {
						//壁その他の判定
						c.wall_col(MAPPTs, get_mine(), use_VR);
						//obj演算
						if (use_VR && (&c == &get_mine())) {
							c.move_vr(/*MAPPTs,*/ DrawPts);
						}
						else {
							c.move_other(MAPPTs, get_mine());
						}
						c.update_anim();
						//
						if (!(use_VR && (&c == &get_mine()))) {
							c.calc_hmd();							//視点取得
							c.calc_gun();							//銃器
						}
						//射撃関連
						c.calc_shot(MAPPTs, DrawPts, this->chara, hit_obj, hit_buf, meds_data, this->item);
					}
					//アイテム演算
					for (auto& g : this->item) {
						g.UpDate(this->item, MAPPTs);
					}
					//アイテム拾う
					for (auto& c : this->chara) {
						c.canget_reset();
						for (auto& g : this->item) {
							g.Get_item_2(c, MAPPTs);
						}
					}
					//空マガジンを削除する
					while (true) {
						bool demag_flag = false;
						size_t demag_id = 0;
						for (auto& i : this->item) {
							if (i.get_ptr_mag() != nullptr && i.get_del_timer() >= 5.f) {
								i.Delete_item();
								demag_id = &i - &this->item[0];
								demag_flag = true;
								break;
							}
						}
						if (demag_flag) {
							this->item.erase(this->item.begin() + demag_id);
						}
						else {
							break;
						}
					}
					//物理演算、アニメーション
					for (auto& c : this->chara) {
						c.Set_Physics();
					}
					//
					for (auto&s : get_mine().sight_) {
						s.Set_reticle();
					}
					//
					for (auto&s : hit_obj) { s.UpDate(); }
				}
				//campos,camvec,camupの指定
				{
					get_mine().Set_cam(MAPPTs, camera_main, this->chara, fov_base, use_VR);
				}
				//ルール保存
				UIparts->Set_rule(RULEparts);
				RULEparts->UpDate();
				//ライト
				get_mine().Set_Light();
				//TPS
				TPSparts->key_TPS.get_in(k_.key_use_ID[15].get_key(0));
				//ミニマップ
				{
					if (TPSparts->key_TPS.on()) {
						minimapparts->Set(this->chara, this->chara[TPSparts->sel_cam], MAPPTs);
					}
					else {
						minimapparts->Set(this->chara, get_mine(), MAPPTs);
					}
				}
				//TPS描画
				TPSparts->Set_info(MAPPTs, this->chara);
				if (TPSparts->key_TPS.on()) {
					//影用意
					DrawPts->Ready_Shadow(TPSparts->camera_TPS.campos, [&] {Shadow_Draw(); }, VGet(2.f, 2.5f, 2.f), VGet(5.f, 2.5f, 5.f));
					//書き込み
					{
						Hostpassparts_TPS->BUF_draw([&](void) { BG_Draw(MAPPTs); }, [&] {DrawPts->Draw_by_Shadow([&] {Main_Draw(MAPPTs); }); }, TPSparts->camera_TPS);	//被写体深度描画
						Hostpassparts_TPS->MAIN_draw();																//最終描画
					}
				}
				//
				{
					cam_info* camp = &camera_main;
					if (TPSparts->key_TPS.on()) {
						camp = &TPSparts->camera_TPS;
					}
					GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), camp->campos, camp->camvec, camp->camup, camp->fov, camp->near_, camp->far_);
					{
						for (auto& c : this->chara) {
							c.Check_CameraViewClip(MAPPTs);
						}
					}
				}
			}
			//終了判定
			if (k_.key_use_ID[11].get_key(0)) {
				return false;
			}
			if (k_.key_use_ID[10].get_key(0)) {
				return false;
			}
			return true;
			//
		}
		void Dispose() {
			for (auto& c : this->chara) {
				c.Delete_chara();
			}
			this->chara.clear();
			for (auto& i : this->item) {
				i.Delete_item();
			}
			this->item.clear();
		}
		void UI_Draw(void) {
			UIparts->Set_Draw(get_mine(), use_VR);
		}
		void BG_Draw(std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>& MAPPTs) {
			MAPPTs->sky_Draw();
		}
		void Shadow_Draw_Far(std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>& MAPPTs) {
			MAPPTs->Shadow_Draw();
		}
		void Shadow_Draw(void) {
			for (auto& c : this->chara) { c.Draw_chara(); }
			for (auto& g : this->item) { g.Draw_item(); }
		}
		void Main_Draw(std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>& MAPPTs) {
			//map
			MAPPTs->main_Draw();
			//命中痕
			for (auto&s : hit_obj) { s.Draw(); }
			//サイト
			for (auto& s : get_mine().sight_) {
				s.Draw_reticle();
			}
			//キャラ
			std::for_each(this->chara.begin(), this->chara.end(), [&](Chara& c) { c.Draw_chara(); });
			//レーザー
			std::for_each(this->chara.begin(), this->chara.end(), [&](Chara& c) { c.Draw_laser(this->chara, MAPPTs, light); });
			//アイテム
			std::for_each(this->item.begin(), this->item.end(), [&](Items& i) { i.Draw_item(get_mine()); });
			//銃弾
			SetFogEnable(FALSE);
			SetUseLighting(FALSE);
			std::for_each(this->chara.begin(), this->chara.end(), [&](Chara& c) { c.Draw_ammo(); });
			SetUseLighting(TRUE);
			SetFogEnable(TRUE);
			//
		}
		void Item_Draw(void) {
			for (auto& c : this->chara) {
				c.Set_Draw_bullet();
			}
			UIparts->item_Draw(this->chara, get_mine(), get_items(), camera_main.campos, use_VR);
		}
		void LAST_Draw(void) {
			if (TPSparts->key_TPS.on()) {
				Hostpassparts_TPS->get_main().DrawGraph(0, 0, true);
			}
			//minimap
			minimapparts->Draw(64, 64);
		}
	};
};