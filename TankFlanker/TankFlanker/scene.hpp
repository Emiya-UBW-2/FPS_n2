#pragma once
class Sceneclass : Mainclass {
private:
	//TPS����
	class TPS_parts {
	public:
		switchs key_TPS;
		float xr_cam = 0.f;
		float yr_cam = 0.f;
		int sel_cam = 0;
		//�J����
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
	//���[��
	class RULE_parts {
	private:
		float Ready = 0.f;
		float timer = 0.f;
	public:
		float gettimer(void) { return timer; }
		float getReady(void) { return Ready; }
		bool getStart(void) { return Ready <= 0.f; }
		bool getend(void) { return timer <= 0.f; }
		void Set(void) {
			Ready = 3.0f;
			timer = 180.f;
		}
		void UpDate(void) {
			if (getStart()) {
				timer -= 1.f / GetFPS();
			}
			else {
				Ready -= 1.f / GetFPS();
			}
			if (getend()) {
				timer = 0.f;
			}
		}

	};
public:
	class MAINLOOP;
	//
	class SELECT {
		//�J����
		cam_info camera_main;
		float fov_base = DX_PI_F / 2;
		//�f�[�^
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
		int sel_p = 0;
		int chang = 0;
		int sel_max = 0;
		int chang_max = 0;
		VECTOR_ref viewparts;
		VECTOR_ref camparts;
		float xrad_t = 30.f;
		float yrad_t = -135.f;
		float range_t = 0.25f;
		float range_tgt = 0.25f;
		switchs Rot;
		int sight1_i = 0;
		int sight2_i = 0;
		bool Start_b = true;
		float rate = 0.1f;
		FontHandle font18;
		Chara* mine_ptr = nullptr;
	public:
		auto& get_camera(void) { return camera_main; }
		//�K�{�i
		void Set_chang_haveto(std::vector<GUNPARTs>&data, const size_t& pts_cat, const size_t& pot_cat, int& chang_t) {
			mine_ptr->delete_parts(pts_cat);

			chang_max = (int)data.size();
			chang_t = std::clamp(chang_t, 0, chang_max - 1);
			parts_p = &data[chang_t];

			mine_ptr->add_parts(parts_p, pts_cat, port_ptr, pot_cat);

			viewparts = port_ptr->obj.frame(port_ptr->rail_f[pot_cat][0].first);
		}
		//��K�{�i
		void Set_chang_needs(std::vector<GUNPARTs>&data, const size_t& pts_cat, const size_t& pot_cat, int& chang_t, const size_t& sel) {
			chang_max = (int)data.size() + 1;
			chang_t = std::clamp(chang_t, 0, chang_max - 1);
			if (chang_t == 0) { parts_p = nullptr; }
			else { parts_p = &data[std::max(chang_t - 1, 0)]; }
			viewparts = port_ptr->obj.frame(port_ptr->rail_f[pot_cat][0].first);
			switch (sel) {
			case 0:
			{
				mine_ptr->delete_parts(pts_cat);
				break;
			}
			case 1:
			{
				if (data.size() == sight1_i && sight1_i > 0) {
					mine_ptr->delete_parts(pts_cat, sight1_i - 1);
				}
				break;
			}
			case 2:
			{
				if (data.size() == sight2_i && sight2_i > 0) {
					mine_ptr->delete_parts(pts_cat, sight2_i - 1);
				}
				break;
			}
			}
			if (chang_t != 0) {
				mine_ptr->add_parts(parts_p, pts_cat, port_ptr, pot_cat);
				switch (sel) {
				case 0:
				{
					break;
				}
				case 1:
				{
					if (chang_t != 0) {
						sight1_i = (int)data.size();
					}
					break;
				}
				case 2:
				{
					if (chang_t != 0) {
						sight2_i = (int)data.size();
					}
					break;
				}
				}
			}
		}
		//��K�{�i3
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
			viewparts = port_ptr->obj.frame(port_ptr->rail_f[pot_cat][0].first);
		}
		//
		SELECT(void) {
			font18 = FontHandle::Create(18, DX_FONTTYPE_EDGE);
		}
		void Set(
			std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>& MAPPTs,
			std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& DrawPts,
			std::unique_ptr<OPTION, std::default_delete<OPTION>>& OPTPTs,
			std::unique_ptr<MAINLOOP, std::default_delete<MAINLOOP>>& MAINLOOPparts
		) {
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
				viewparts.clear();
				camparts.clear();
				xrad_t = 30.f;
				yrad_t = -135.f;
				range_t = 0.25f;
				range_tgt = 0.25f;
				sight1_i = 0;
				sight2_i = 0;
				Start_b = true;
				rate = 0.1f;
			}
			{
				mine_ptr = &MAINLOOPparts->get_mine();
			}
			SetUseMaskScreenFlag(FALSE);//���J�X�^����ʂŃG�t�F�N�g���o�Ȃ��Ȃ邽�ߓ����
			SetMousePoint(deskx / 2, desky / 2);											//
			fov_base = deg2rad(DrawPts->use_vr ? 90 : OPTPTs->Fov);
			camera_main.set_cam_info(fov_base, 0.1f, 200.f);//1P
			//���C�e�B���O
			DrawPts->Set_Light_Shadow(MAPPTs->map_col_get().mesh_maxpos(0), MAPPTs->map_col_get().mesh_minpos(0), VGet(0.5f, -0.5f, 0.5f), [&] {});
			SetGlobalAmbientLight(GetColorF(0.42f, 0.41f, 0.40f, 0.0f));
			//�K�v�p�[�c����
			{
				//
				{
					parts_p = &MAINLOOPparts->get_mag_data()[0];
					mine_ptr->add_parts(parts_p, EnumGunParts::PARTS_MAGAZINE);
				}
				//grip
				{
					parts_p = &MAINLOOPparts->get_grip_data()[0];
					mine_ptr->add_parts(parts_p, EnumGunParts::PARTS_GRIP, &mine_ptr->base, EnumAttachPoint::GRIP_BASE);
				}
				//uperhandguard
				{
					parts_p = &MAINLOOPparts->get_uperhandguard_data()[0];
					mine_ptr->add_parts(parts_p, EnumGunParts::PARTS_UPER_HGUARD, &mine_ptr->base, EnumAttachPoint::UPER_HANDGUARD);
				}
				//underhandguard
				{
					parts_p = &MAINLOOPparts->get_underhandguard_data()[0];
					mine_ptr->add_parts(parts_p, EnumGunParts::PARTS_UNDER_HGUARD, &mine_ptr->base, EnumAttachPoint::UNDER_HANDGUARD);
				}
			}
		}
		bool UpDate(
			std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>& MAPPTs, 
			std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& DrawPts,
			std::unique_ptr<MAINLOOP, std::default_delete<MAINLOOP>>& MAINLOOPparts
		) {
			{
				bool changef = false;
				//���Z
				{
					{
						mine_ptr->mat_gun = MGetIdent();
						mine_ptr->pos_gun = VGet(0, 0, 0);
						mine_ptr->Set_gun();
						//
						mine_ptr->mat_mag = mine_ptr->mat_gun;
						mine_ptr->pos_mag = mine_ptr->base.obj.frame(mine_ptr->base.thisparts->frame_s.magazine2_f.first);
						mine_ptr->magazine.obj.SetMatrix(mine_ptr->mat_mag* MATRIX_ref::Mtrans(mine_ptr->pos_mag));
						mine_ptr->key_.wkey = (CheckHitKey(KEY_INPUT_W) != 0);
						mine_ptr->key_.skey = (CheckHitKey(KEY_INPUT_S) != 0);
						mine_ptr->key_.akey = (CheckHitKey(KEY_INPUT_A) != 0);
						mine_ptr->key_.dkey = (CheckHitKey(KEY_INPUT_D) != 0);
						mine_ptr->key_.shoot = ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0);
						up.get_in(mine_ptr->key_.wkey);
						down.get_in(mine_ptr->key_.skey);
						left.get_in(mine_ptr->key_.akey);
						right.get_in(mine_ptr->key_.dkey);
						shot.get_in(mine_ptr->key_.shoot);
						Start.get_in(CheckHitKey(KEY_INPUT_SPACE) != 0);
						Rot.get_in((GetMouseInput() & MOUSE_INPUT_MIDDLE) != 0);
						if (Start_b) {
							changef = true;
							for (auto& s : mine_ptr->base.thisparts->select) {
								if (s == 1) {
									mine_ptr->gun_stat_now->select = uint8_t(&s - &mine_ptr->base.thisparts->select[0]);
									break;
								}
							}
							Start_b = false;
						}

						if (up.push()) {
							sel_p--;
							chang = 0;
							changef = true;
						}
						if (down.push()) {
							sel_p++;
							chang = 0;
							changef = true;
						}
						if (sel_p < 0) { sel_p = sel_max - 1; }
						if (sel_p > sel_max - 1) { sel_p = 0; }

						if (left.push()) {
							chang--;
							changef = true;
						}
						if (right.push()) {
							chang++;
							changef = true;
						}
						if (chang < 0) { chang = chang_max - 1; }
						if (chang > chang_max - 1) { chang = 0; }
					}
					if (changef) {
						port_ptr = nullptr;
						sel_max = 0;
						//grip
						{
							if (sel_p == sel_max) {
								port_ptr = &mine_ptr->base;
								port_cat = EnumAttachPoint::GRIP_BASE;
								parts_cat = EnumGunParts::PARTS_GRIP;
								Set_chang_haveto(MAINLOOPparts->get_grip_data(), parts_cat, port_cat, chang);
							}
							sel_max++;
						}
						//uperhandguard
						{
							if (sel_p == sel_max) {
								port_ptr = &mine_ptr->base;
								port_cat = EnumAttachPoint::UPER_HANDGUARD;
								parts_cat = EnumGunParts::PARTS_UPER_HGUARD;
								Set_chang_haveto(MAINLOOPparts->get_uperhandguard_data(), parts_cat, port_cat, chang);
							}
							sel_max++;
						}
						//underhandguard
						{
							if (sel_p == sel_max) {
								port_ptr = &mine_ptr->base;
								port_cat = EnumAttachPoint::UNDER_HANDGUARD;
								parts_cat = EnumGunParts::PARTS_UNDER_HGUARD;
								Set_chang_haveto(MAINLOOPparts->get_underhandguard_data(), parts_cat, port_cat, chang);
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
									Set_chang_needs(MAINLOOPparts->get_foregrip_data(), parts_cat, port_cat, chang, 0);
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
										parts_cat = (chang < MAINLOOPparts->get_light_data().size() + 1) ? EnumGunParts::PARTS_LIGHT : EnumGunParts::PARTS_LASER;
									}
									mine_ptr->delete_parts(EnumGunParts::PARTS_LIGHT);
									mine_ptr->delete_parts(EnumGunParts::PARTS_LASER);
									Set_chang_need3(MAINLOOPparts->get_light_data(), MAINLOOPparts->get_laser_data(), parts_cat, port_cat, chang);
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
									else { parts_p = &MAINLOOPparts->get_mount_data()[std::max(chang - 1, 0)]; }
									mine_ptr->delete_parts(parts_cat, 0);
									if (chang != 0) {
										mine_ptr->add_parts(parts_p, parts_cat, port_ptr, port_cat);
									}
									viewparts = port_ptr->obj.frame(port_ptr->rail_f[port_cat][0].first);
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
											else { parts_p = &MAINLOOPparts->get_mount_data()[chang]; }
											if (mine_ptr->mount_.size() == 2) {
												mine_ptr->delete_parts(parts_cat, 1);
											}
											if (chang != 0) {
												mine_ptr->add_parts(parts_p, parts_cat, port_ptr, port_cat);
											}
											viewparts = port_ptr->obj.frame(port_ptr->rail_f[port_cat][0].first);
										}
									}
									sel_max++;
								}
							}
						}
						//�}�Y��
						{
							if (sel_p == sel_max) {
								port_ptr = &mine_ptr->base;
								port_cat = EnumAttachPoint::MAZZULE_BASE;
								parts_cat = EnumGunParts::PARTS_MAZZULE;
								Set_chang_needs(MAINLOOPparts->get_mazzule_data(), parts_cat, port_cat, chang, 0);
							}
							sel_max++;
						}
						//�_�X�g�J�o�[
						{
							if (sel_p == sel_max) {
								port_ptr = &mine_ptr->base;
								port_cat = EnumAttachPoint::DUSTCOVER_BASE;
								parts_cat = EnumGunParts::PARTS_DUSTCOVER;
								Set_chang_needs(MAINLOOPparts->get_dustcover_data(), parts_cat, port_cat, chang, 0);
							}
							sel_max++;
						}
						//�X�g�b�N
						{
							if (sel_p == sel_max) {
								port_ptr = &mine_ptr->base;
								port_cat = EnumAttachPoint::STOCK_BASE;
								parts_cat = EnumGunParts::PARTS_STOCK;
								Set_chang_needs(MAINLOOPparts->get_stock_data(), parts_cat, port_cat, chang, 0);
							}
							sel_max++;
						}
						//sight
						{
							port_ptr = &mine_ptr->uperhandguard;
							port_cat = EnumAttachPoint::UPER_RAIL;
							if (port_ptr->rail_f[port_cat][0].first > 0) {
								if (sel_p == sel_max) {
									parts_cat = EnumGunParts::PARTS_SIGHT;
									Set_chang_needs(MAINLOOPparts->get_sight_data(), parts_cat, port_cat, chang, 1);
								}
								sel_max++;
							}
						}
						//sight2
						{
							if (mine_ptr->mount_.size() > 1) {
								port_ptr = &mine_ptr->mount_[1];
								port_cat = EnumAttachPoint::UPER_RAIL;
								if (port_ptr->rail_f[port_cat][0].first > 0) {
									if (sel_p == sel_max) {
										parts_cat = EnumGunParts::PARTS_SIGHT;
										Set_chang_needs(MAINLOOPparts->get_sight_data(), parts_cat, port_cat, chang, 2);
									}
									sel_max++;
								}
							}
						}
						//
						{
							xrad_t = viewparts.y()*1000.f;
							yrad_t = ((viewparts.x() > 0.f) ? 90.f - viewparts.z()*100.f : -90.f + viewparts.z()*100.f);
						}
						//
					}
					//
					if (shot.push()) {
						if (!mine_ptr->gunf) {
							//���
							mine_ptr->calc_cart(rate);
							//�G�t�F�N�g
							mine_ptr->calc_shot_effect();
						}
						//
						mine_ptr->gunf = true;
					}
					//
					mine_ptr->set_select();
					//
					mine_ptr->set_shot(rate);
					//
					mine_ptr->base.obj.work_anime();

					//��䰂̏���
					mine_ptr->update_cart(MAPPTs);
					mine_ptr->update_effect(DrawPts);
				}
				//campos,camvec,camup�̎w��
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
						auto tmp = viewparts;
						if (std::abs(yrad_t) > 90) {
							tmp = (
								(mine_ptr->mazzule.attach) ?
								mine_ptr->mazzule.obj.frame(mine_ptr->mazzule.mazzule_frame.first) :
								mine_ptr->base.obj.frame(mine_ptr->base.thisparts->frame_s.mazzule_f.first)
								) - tmp;
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
			//�I������
			if (CheckHitKey(KEY_INPUT_ESCAPE) != 0) {
				return false;
			}
			if (Start.push()) {
				return false;
			}
			return true;
			//
		}
		void Dispose(void) {
		}
		void UI_Draw(
			std::unique_ptr<MAINLOOP, std::default_delete<MAINLOOP>>& MAINLOOPparts
		) {

			font18.DrawStringFormat(100, 300, GetColor(255, 0, 0), "total : %d / %d", sel_p + 1, sel_max);
			font18.DrawStringFormat(100, 350, GetColor(255, 0, 0), "chang : %d / %d", chang + 1, chang_max);

			int i = 0;
			for (auto& m : mine_ptr->mount_) {
				font18.DrawString(400, 300 + i, m.name, GetColor(255, 0, 0)); i += 20;
			}

			i = 0;
			for (auto& m : mine_ptr->sight_) {
				font18.DrawString(700, 300 + i, m.name, GetColor(255, 0, 0)); i += 20;
			}

			if (parts_cat != SIZE_MAX) {
				int cnt = 0;
				switch (parts_cat) {
				case EnumGunParts::PARTS_MAZZULE:
				{
					font18.DrawString(100, 375, "MAZZULE", GetColor(255, 0, 0));
					{
						if (parts_p == nullptr) {
							font18.DrawString(100 + 250 * cnt, 400, "NONE", GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, "NONE", GetColor(255, 255, 0));
						}
						cnt++;
					}
					for (auto& p : MAINLOOPparts->get_mazzule_data()) {
						if (&p == parts_p) {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.get_name(), GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.get_name(), GetColor(255, 255, 0));
						}
						cnt++;
					}
					break;
				}
				case EnumGunParts::PARTS_GRIP:
				{
					font18.DrawString(100, 375, "GRIP", GetColor(255, 0, 0));
					{
						if (parts_p == nullptr) {
							font18.DrawString(100 + 250 * cnt, 400, "NONE", GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, "NONE", GetColor(255, 255, 0));
						}
						cnt++;
					}
					for (auto& p : MAINLOOPparts->get_grip_data()) {
						if (&p == parts_p) {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.get_name(), GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.get_name(), GetColor(255, 255, 0));
						}
						cnt++;
					}
					break;
				}
				case EnumGunParts::PARTS_UPER_HGUARD:
				{
					font18.DrawString(100, 375, "UPER HANDGUARD", GetColor(255, 0, 0));
					{
						if (parts_p == nullptr) {
							font18.DrawString(100 + 250 * cnt, 400, "NONE", GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, "NONE", GetColor(255, 255, 0));
						}
						cnt++;
					}
					for (auto& p : MAINLOOPparts->get_uperhandguard_data()) {
						if (&p == parts_p) {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.get_name(), GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.get_name(), GetColor(255, 255, 0));
						}
						cnt++;
					}
					break;
				}
				case EnumGunParts::PARTS_UNDER_HGUARD:
				{
					font18.DrawString(100, 375, "UNDER HANDGUARD", GetColor(255, 0, 0));
					{
						if (parts_p == nullptr) {
							font18.DrawString(100 + 250 * cnt, 400, "NONE", GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, "NONE", GetColor(255, 255, 0));
						}
						cnt++;
					}
					for (auto& p : MAINLOOPparts->get_underhandguard_data()) {
						if (&p == parts_p) {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.get_name(), GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.get_name(), GetColor(255, 255, 0));
						}
						cnt++;
					}
					break;
				}
				case EnumGunParts::PARTS_DUSTCOVER:
				{
					font18.DrawString(100, 375, "DUSTCOVER", GetColor(255, 0, 0));
					{
						if (parts_p == nullptr) {
							font18.DrawString(100 + 250 * cnt, 400, "NONE", GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, "NONE", GetColor(255, 255, 0));
						}
						cnt++;
					}
					for (auto& p : MAINLOOPparts->get_dustcover_data()) {
						if (&p == parts_p) {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.get_name(), GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.get_name(), GetColor(255, 255, 0));
						}
						cnt++;
					}
					break;
				}
				case EnumGunParts::PARTS_STOCK:
				{
					font18.DrawString(100, 375, "STOCK", GetColor(255, 0, 0));
					{
						if (parts_p == nullptr) {
							font18.DrawString(100 + 250 * cnt, 400, "NONE", GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, "NONE", GetColor(255, 255, 0));
						}
						cnt++;
					}
					for (auto& p : MAINLOOPparts->get_stock_data()) {
						if (&p == parts_p) {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.get_name(), GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.get_name(), GetColor(255, 255, 0));
						}
						cnt++;
					}
					break;
				}
				case EnumGunParts::PARTS_LASER:
				{
					font18.DrawString(100, 375, "LASER", GetColor(255, 0, 0));
					{
						if (parts_p == nullptr) {
							font18.DrawString(100 + 250 * cnt, 400, "NONE", GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, "NONE", GetColor(255, 255, 0));
						}
						cnt++;
					}
					for (auto& p : MAINLOOPparts->get_laser_data()) {
						if (&p == parts_p) {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.get_name(), GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.get_name(), GetColor(255, 255, 0));
						}
						cnt++;
					}
					break;
				}
				case EnumGunParts::PARTS_LIGHT:
				{
					font18.DrawString(100, 375, "LIGHT", GetColor(255, 0, 0));
					{
						if (parts_p == nullptr) {
							font18.DrawString(100 + 250 * cnt, 400, "NONE", GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, "NONE", GetColor(255, 255, 0));
						}
						cnt++;
					}
					for (auto& p : MAINLOOPparts->get_light_data()) {
						if (&p == parts_p) {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.get_name(), GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.get_name(), GetColor(255, 255, 0));
						}
						cnt++;
					}
					break;
				}
				case EnumGunParts::PARTS_FOREGRIP:
				{
					font18.DrawString(100, 375, "FOREGRIP", GetColor(255, 0, 0));
					{
						if (parts_p == nullptr) {
							font18.DrawString(100 + 250 * cnt, 400, "NONE", GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, "NONE", GetColor(255, 255, 0));
						}
						cnt++;
					}
					for (auto& p : MAINLOOPparts->get_foregrip_data()) {
						if (&p == parts_p) {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.get_name(), GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.get_name(), GetColor(255, 255, 0));
						}
						cnt++;
					}
					break;
				}
				case EnumGunParts::PARTS_MOUNT:
				{
					font18.DrawString(100, 375, "MOUNT", GetColor(255, 0, 0));
					{
						if (parts_p == nullptr) {
							font18.DrawString(100 + 250 * cnt, 400, "NONE", GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, "NONE", GetColor(255, 255, 0));
						}
						cnt++;
					}
					for (auto& p : MAINLOOPparts->get_mount_data()) {
						if (&p == parts_p) {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.get_name(), GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.get_name(), GetColor(255, 255, 0));
						}
						cnt++;
					}
					break;
				}
				case EnumGunParts::PARTS_SIGHT:
				{
					font18.DrawString(100, 375, "SIGHT", GetColor(255, 0, 0));
					{
						if (parts_p == nullptr) {
							font18.DrawString(100 + 250 * cnt, 400, "NONE", GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, "NONE", GetColor(255, 255, 0));
						}
						cnt++;
					}
					for (auto& p : MAINLOOPparts->get_sight_data()) {
						if (&p == parts_p) {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.get_name(), GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.get_name(), GetColor(255, 255, 0));
						}
						cnt++;
					}
					break;
				}
				default:
					break;
				}
			}

			font18.DrawStringFormat(100, 700, GetColor(255, 0, 0), "weigt  : %3.1f", mine_ptr->get_per().weight);
			font18.DrawStringFormat(100, 725, GetColor(255, 0, 0), "recoil : %3.1f", mine_ptr->get_per().recoil);

			font18.DrawString(100, 525, "����   :Parts Change", GetColor(255, 0, 0));
			font18.DrawString(100, 550, "����   :Parts Select", GetColor(255, 0, 0));
			font18.DrawString(100, 575, "SPACE  :Go Battle", GetColor(255, 0, 0));
			font18.DrawString(100, 600, (Rot.on()) ? "RANGE  :FREE" : "RANGE  :FIXED", GetColor(255, 0, 0));
		}
		void BG_Draw(void) {
			DrawBox(0, 0, 1920, 1080, GetColor(128, 128, 128), TRUE);
		}
		void Shadow_Draw(void) {
			mine_ptr->Draw_gun();
			if (mine_ptr->magazine.attach) {
				mine_ptr->magazine.obj.DrawModel();
			}
		}
		void Main_Draw(void) {
			mine_ptr->Draw_gun();
			if (mine_ptr->magazine.attach) {
				mine_ptr->magazine.obj.DrawModel();
			}
		}
	};
	//
	class MAINLOOP {
		//�J����
		cam_info camera_main;
		float fov_base = DX_PI_F / 2;
		bool use_VR = false;
		//�f�[�^
		MV1 body_obj, body_col;						//�g�̃��f��
		MV1 hit_pic;								//�e��
		GraphHandle light;							//���C�g�e�N�X�`��
		std::vector<GUNPARTs> mazzule_data;			//GUN�f�[�^
		std::vector<GUNPARTs> grip_data;			//GUN�f�[�^
		std::vector<GUNPARTs> uperhandguard_data;	//GUN�f�[�^
		std::vector<GUNPARTs> underhandguard_data;	//GUN�f�[�^
		std::vector<GUNPARTs> stock_data;			//GUN�f�[�^
		std::vector<GUNPARTs> dustcover_data;		//GUN�f�[�^
		std::vector<GUNPARTs> sight_data;			//GUN�f�[�^
		std::vector<GUNPARTs> foregrip_data;		//GUN�f�[�^
		std::vector<GUNPARTs> light_data;			//GUN�f�[�^
		std::vector<GUNPARTs> laser_data;			//GUN�f�[�^
		std::vector<GUNPARTs> mount_data;			//GUN�f�[�^
		std::vector<GUNPARTs> gun_data;				//GUN�f�[�^
		std::vector<GUNPARTs> mag_data;				//GUN�f�[�^
		std::vector<Meds> meds_data;				//GUN�f�[�^
		//�I�u�W�F
		std::vector<Chara> chara;					//�L����
		std::vector<Items> item;					//�A�C�e��
		std::vector<Hit> hit_obj;					//�e��
		size_t hit_buf = 0;							//
		//
		std::unique_ptr<HostPassEffect, std::default_delete<HostPassEffect>> Hostpassparts_TPS;
		std::unique_ptr<TPS_parts, std::default_delete<TPS_parts>> TPSparts;
		std::unique_ptr<UIclass::UI, std::default_delete<UIclass::UI>> UIparts;
		std::unique_ptr<RULE_parts, std::default_delete<RULE_parts>> RULEparts;
		std::unique_ptr<MAPclass::MiniMap, std::default_delete<MAPclass::MiniMap>> minimapparts;
	public:
		std::vector<Chara>& get_chara(void) { return this->chara; }
		std::vector<Items>& get_item(void) { return this->item; }
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
		MAINLOOP(
			std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& DrawPts,
			std::unique_ptr<OPTION, std::default_delete<OPTION>>& OPTPTs,
			std::unique_ptr<UIclass::UI_Load, std::default_delete<UIclass::UI_Load>>& UI_LOADPTs
		) {
			//
			use_VR = DrawPts->use_vr;
			//
			UIparts = std::make_unique<UIclass::UI>();
			TPSparts = std::make_unique<TPS_parts>();
			RULEparts = std::make_unique<RULE_parts>();
			Hostpassparts_TPS = std::make_unique<HostPassEffect>(OPTPTs->DoF, OPTPTs->Bloom, deskx, desky);	//�z�X�g�p�X�G�t�F�N�g(�t���X�N���[�������ATPS�p)
			minimapparts = std::make_unique<MAPclass::MiniMap>();											//�~�j�}�b�v
			{
				//model
				light = GraphHandle::Load("data/light.png");					//���C�g
				MV1::Load("data/model/body2/model.mv1", &this->body_obj, true);	//�g��
				MV1::Load("data/model/body2/col.mv1", &this->body_col, true);	//�g��col
				MV1::Load("data/model/hit/model.mv1", &this->hit_pic, true);	//�e��
				//
				this->mazzule_data.resize(2);
				this->mazzule_data[0].mod.Ready("data/parts/mazzule/", "AK_6P1_Compensator");
				this->mazzule_data[1].mod.Ready("data/parts/mazzule/", "sup");
				this->grip_data.resize(3);
				this->grip_data[0].mod.Ready("data/parts/grip/", "AK_6P1_Wood_Grip");
				this->grip_data[1].mod.Ready("data/parts/grip/", "AK_original_Grip");
				this->grip_data[2].mod.Ready("data/parts/grip/", "AK_Zenit_RK3_Grip");
				this->uperhandguard_data.resize(4);
				this->uperhandguard_data[0].mod.Ready("data/parts/uper_handguard/", "AK_6P1_Uper_Handguard");
				this->uperhandguard_data[1].mod.Ready("data/parts/uper_handguard/", "AK_Original_Polymer_Uper_Handguard");
				this->uperhandguard_data[2].mod.Ready("data/parts/uper_handguard/", "AK_Steel_Uper_Handguard");
				this->uperhandguard_data[3].mod.Ready("data/parts/uper_handguard/", "AK_Rail_Band");
				this->underhandguard_data.resize(3);
				this->underhandguard_data[0].mod.Ready("data/parts/under_handguard/", "AK_6P1_Under_Handguard");
				this->underhandguard_data[1].mod.Ready("data/parts/under_handguard/", "AK_Original_Polymer_Under_Handguard");
				this->underhandguard_data[2].mod.Ready("data/parts/under_handguard/", "AK_Zenit_B10M_Under_Handguard");
				this->mount_data.resize(2);
				this->mount_data[0].mod.Ready("data/parts/mount/", "AK_Dovetail_Mount");
				this->mount_data[1].mod.Ready("data/parts/mount/", "AK_SideRailMount");
				this->stock_data.resize(1);
				this->stock_data[0].mod.Ready("data/parts/stock/", "AK_6P1_Stock");
				this->dustcover_data.resize(1);
				this->dustcover_data[0].mod.Ready("data/parts/dustcover/", "AK_6P1_Dustcover");
				this->sight_data.resize(2);
				this->sight_data[0].mod.Ready("data/parts/sight/", "Eotech_XPS3_HoloSight");
				this->sight_data[1].mod.Ready("data/parts/sight/", "AIMPOINT_T1_DotSight");
				this->foregrip_data.resize(2);
				this->foregrip_data[0].mod.Ready("data/parts/foregrip/", "Standard_Vertical_Foregrip");
				this->foregrip_data[1].mod.Ready("data/parts/foregrip/", "Bipod_Vertical_Foregrip");
				this->light_data.resize(1);
				this->light_data[0].mod.Ready("data/parts/light/", "Tactical_Flashlight");
				this->laser_data.resize(1);
				this->laser_data[0].mod.Ready("data/parts/laser/", "NcSTAR_Laser");
				this->gun_data.resize(1);
				this->gun_data[0].mod.Ready("data/gun/", "AKM");
				//MAG�f�[�^
				this->mag_data.resize(1);
				this->mag_data[0].mod.Ready("data/mag/", "AK_55_Magazine");
				//MED�f�[�^
				this->meds_data.resize(1);
				this->meds_data[0].mod.Ready("data/medkit/", "AIDkit");
			}
			//���[�h���1
			UI_LOADPTs->load_window("�A�C�e���I�u�W�F�N�g");
			{
				//GUN�f�[�^2
				for (auto& g : this->mazzule_data) { g.Set_datas(&g - &this->mazzule_data[0], EnumGunParts::PARTS_MAZZULE); }
				for (auto& g : this->grip_data) { g.Set_datas(&g - &this->grip_data[0], EnumGunParts::PARTS_NONE); }
				for (auto& g : this->uperhandguard_data) { g.Set_datas(&g - &this->uperhandguard_data[0], EnumGunParts::PARTS_NONE); }
				for (auto& g : this->underhandguard_data) { g.Set_datas(&g - &this->underhandguard_data[0], EnumGunParts::PARTS_NONE); }
				for (auto& g : this->mount_data) { g.Set_datas(&g - &this->mount_data[0], EnumGunParts::PARTS_NONE); }
				for (auto& g : this->stock_data) { g.Set_datas(&g - &this->stock_data[0], EnumGunParts::PARTS_NONE); }
				for (auto& g : this->dustcover_data) { g.Set_datas(&g - &this->dustcover_data[0], EnumGunParts::PARTS_NONE); }
				for (auto& g : this->light_data) { g.Set_datas(&g - &this->light_data[0], EnumGunParts::PARTS_NONE); }
				for (auto& g : this->laser_data) { g.Set_datas(&g - &this->laser_data[0], EnumGunParts::PARTS_NONE); }
				for (auto& g : this->foregrip_data) { g.Set_datas(&g - &this->foregrip_data[0], EnumGunParts::PARTS_NONE); }
				for (auto& g : this->sight_data) { g.Set_datas(&g - &this->sight_data[0], EnumGunParts::PARTS_SIGHT); }
				for (auto& g : this->mag_data) { g.Set_datas(&g - &this->mag_data[0], EnumGunParts::PARTS_MAGAZINE); }
				for (auto& g : this->gun_data) { g.Set_datas(&g - &this->gun_data[0], EnumGunParts::PARTS_BASE); }
				//MED�f�[�^2
				for (auto& g : this->meds_data) { g.Set_datas(&g - &this->meds_data[0]); }
				//�e��
				this->hit_buf = 0;
				this->hit_obj.resize(24);
				for (auto& h : this->hit_obj) { h.Set(this->hit_pic); }
			}
			//���[�h���2
			UI_LOADPTs->load_window("�A�C�e���f�[�^");
		}
		void first(std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>& MAPPTs) {
			//�L�����ݒ�
			this->chara.resize(MAPPTs->get_spawn_point().size());
			for (auto& c : this->chara) {
				c.Set(gun_data, 0, body_obj, body_col);
			}
		}
		void Set(
			std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>& MAPPTs,
			std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& DrawPts, 
			std::unique_ptr<OPTION, std::default_delete<OPTION>>& OPTPTs
		) {
			for (auto& c : this->chara) {
				//�J�X�^��attach
				if (&c != &get_mine()) {
					//magazine
					c.add_parts(&mag_data[0], EnumGunParts::PARTS_MAGAZINE);
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
			//���C�e�B���O
			DrawPts->Set_Light_Shadow(MAPPTs->map_col_get().mesh_maxpos(0), MAPPTs->map_col_get().mesh_minpos(0), VGet(0.5f, -0.5f, 0.5f), [&] { MAPPTs->Shadow_Draw(); });
			SetGlobalAmbientLight(GetColorF(0.42f, 0.41f, 0.40f, 0.0f));
			//
			MAPPTs->Start_map();															//��
			std::for_each(this->chara.begin(), this->chara.end(), [&](Chara& c) { c.Start(MAPPTs); });//�v���C���[����ϐ��Q
			SetMousePoint(deskx / 2, desky / 2);											//
			fov_base = deg2rad(use_VR ? 90 : OPTPTs->Fov);
			camera_main.set_cam_info(fov_base, 0.1f, 200.f);//1P
			TPSparts->Set(OPTPTs->Fov);															//TPS
			RULEparts->Set();																//���[��
		}
		bool UpDate(
			std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>& MAPPTs,
			std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& DrawPts
		) {
			{
				//���Z
				{
					//���W�擾
					get_mine().set_waist(DrawPts);
					for (auto& c : this->chara) {
						c.Set_alive(this->item, MAPPTs);
					}

					//�v���C���[����
					{
						for (auto& c : this->chara) {
							if (&c == &get_mine()) {
								//mine
								if (use_VR) {
									c.operation_VR(DrawPts, !RULEparts->getStart() || RULEparts->getend());
								}
								else {
									c.operation(!RULEparts->getStart() || RULEparts->getend(), camera_main.fov / fov_base);
								}
							}
							else {
								//cpu
								c.operation_NPC(MAPPTs, this->chara, !RULEparts->getStart() || RULEparts->getend());
							}
							//common
							c.operation_2();
						}
					}
					//
					for (auto& c : this->chara) {
						//�ǂ��̑��̔���
						c.wall_col(MAPPTs, get_mine(), use_VR);
						//obj���Z
						if (use_VR && (&c == &get_mine())) {
							c.move_vr(MAPPTs, DrawPts);
						}
						else {
							c.move_other(MAPPTs, get_mine());
						}
						c.update_anim();
						//
						if (!(use_VR && (&c == &get_mine()))) {
							c.calc_hmd();							//���_�擾
							c.calc_gun();							//�e��
						}
						//�ˌ��֘A
						c.calc_shot(MAPPTs, DrawPts, this->chara, hit_obj, hit_buf, meds_data, this->item);
					}
					//�A�C�e�����Z
					for (auto& g : this->item) {
						g.UpDate(this->item, MAPPTs);
					}
					//�A�C�e���E��
					for (auto& c : this->chara) {
						c.canget_gunitem = false;
						c.canget_magitem = false;
						c.canget_meditem = false;
						for (auto& g : this->item) {
							g.Get_item_2(c, MAPPTs);
						}
					}
					//��}�K�W�����폜����
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
					//�������Z�A�A�j���[�V����
					for (auto& c : this->chara) {
						c.Set_Physics();
					}
					//
					for (auto&s : get_mine().sight_) {
						s.Set_reticle();
					}
				}
				//campos,camvec,camup�̎w��
				{
					get_mine().Set_cam(MAPPTs, camera_main, this->chara, fov_base, use_VR);
				}
				//���[���ۑ�
				UIparts->Set_rule(RULEparts->getReady(), RULEparts->gettimer());
				RULEparts->UpDate();
				//���C�g
				get_mine().Set_Light();
				//TPS
				TPSparts->key_TPS.get_in(CheckHitKey(KEY_INPUT_LCONTROL) != 0);
				//�~�j�}�b�v
				{
					if (TPSparts->key_TPS.on()) {
						minimapparts->Set(this->chara, this->chara[TPSparts->sel_cam], MAPPTs);
					}
					else {
						minimapparts->Set(this->chara, get_mine(), MAPPTs);
					}
				}
				//TPS�`��
				TPSparts->Set_info(MAPPTs, this->chara);
				if (TPSparts->key_TPS.on()) {
					//�e�p��
					DrawPts->Ready_Shadow(TPSparts->camera_TPS.campos, [&] {Shadow_Draw(); }, VGet(2.f, 2.5f, 2.f), VGet(5.f, 2.5f, 5.f));
					//��������
					{
						Hostpassparts_TPS->BUF_draw([&](void) { BG_Draw(MAPPTs); }, [&] {DrawPts->Draw_by_Shadow([&] {Main_Draw(MAPPTs); }); }, TPSparts->camera_TPS);	//��ʑ̐[�x�`��
						Hostpassparts_TPS->MAIN_draw();																//�ŏI�`��
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
			//�I������
			if (CheckHitKey(KEY_INPUT_ESCAPE) != 0) {
				return false;
			}
			if (CheckHitKey(KEY_INPUT_O) != 0) {
				return false;
			}
			return true;
			//
		}
		void Dispose(
			std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>& MAPPTs
		) {
			for (auto& c : this->chara) {
				c.Delete_chara();
			}
			this->chara.clear();
			for (auto& i : this->item) {
				i.Delete_item();
			}
			this->item.clear();
			MAPPTs->Delete_map();
		}
		void UI_Draw(void) {
			UIparts->Set_Draw(get_mine(), use_VR);
		}
		void BG_Draw(
			std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>& MAPPTs
		) {
			MAPPTs->sky_Draw();
		}
		void Shadow_Draw(void) {
			for (auto& c : this->chara) { c.Draw_chara(); }
			for (auto& g : this->item) { g.Draw_item(); }
		}
		void Main_Draw(
			std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>& MAPPTs
		) {
			//map
			MAPPTs->main_Draw();
			//������
			for (auto&s : hit_obj) { s.Draw(); }
			//�T�C�g
			for (auto& s : get_mine().sight_) {
				s.Draw_reticle(MAPPTs);
			}
			//�L����
			std::for_each(this->chara.begin(), this->chara.end(), [&](Chara& c) { c.Draw_chara(); });
			//���[�U�[
			std::for_each(this->chara.begin(), this->chara.end(), [&](Chara& c) { c.Draw_laser(this->chara, MAPPTs, light); });
			//�A�C�e��
			std::for_each(this->item.begin(), this->item.end(), [&](Items& i) { i.Draw_item(get_mine()); });
			//�e�e
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
			UIparts->item_Draw(this->chara, get_mine(), get_item(), camera_main.campos, use_VR);
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