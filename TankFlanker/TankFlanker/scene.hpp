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
		template<class Y, class D>
		void Set_info(std::unique_ptr<Y, D>& MAPPTs, std::vector<Chara>&chara) {
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
						VECTOR_ref vec_2 = (chara[this->sel_cam].body.frame(chara[this->sel_cam].frame_s.head_f.first) - this->camera_TPS.campos).Norm();
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
	//ルール
	class rule {
	private:
		float Ready = 0.f;
		float timer = 0.f;
	public:
		float gettimer() {
			return timer;
		}

		float getReady() {
			return Ready;
		}

		bool getStart() {
			return Ready <= 0.f;
		}
		bool getend() {
			return timer <= 0.f;
		}

		void Set() {
			Ready = 3.0f;
			timer = 180.f;
		}

		void update() {
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
		//カメラ
		cam_info camera_main;
		//
		switchs up;
		switchs down;
		switchs left;
		switchs right;
		switchs shot;
		switchs Start;
		gunparts* parts_p = nullptr;

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
		auto& get_camera() { return camera_main; }
		//
		SELECT() {
			font18 = FontHandle::Create(18, DX_FONTTYPE_EDGE);
		}
		template<class Y2, class D2>
		void Set(std::unique_ptr<Y2, D2>& MAPPTs, std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& DrawPts, std::unique_ptr<OPTION, std::default_delete<OPTION>>& OPTPTs, std::unique_ptr<MAINLOOP, std::default_delete<MAINLOOP>>& MAINLOOPparts) {
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
			SetUseMaskScreenFlag(FALSE);//←カスタム画面でエフェクトが出なくなるため入れる
			SetMousePoint(deskx / 2, desky / 2);											//
			camera_main.set_cam_info(deg2rad(DrawPts->use_vr ? 90 : OPTPTs->fov_pc), 0.1f, 200.f);//1P
			//ライティング
			DrawPts->Set_Light_Shadow(MAPPTs->map_col_get().mesh_maxpos(0), MAPPTs->map_col_get().mesh_minpos(0), VGet(0.5f, -0.5f, 0.5f), [&] {});
			SetGlobalAmbientLight(GetColorF(0.42f, 0.41f, 0.40f, 0.0f));
			//必要パーツつける
			{
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
		//必須品
		void Set_chang_haveto(std::vector<gunparts>&data, const size_t& pts_cat, const size_t& pot_cat, int& chang_t) {
			mine_ptr->delete_parts(pts_cat);

			chang_max = (int)data.size();
			chang_t = std::clamp(chang_t, 0, chang_max - 1);
			parts_p = &data[chang_t];

			mine_ptr->add_parts(parts_p, pts_cat, port_ptr, pot_cat);

			viewparts = port_ptr->obj.frame(port_ptr->rail_f[pot_cat][0].first);
		}
		//非必須品
		void Set_chang_needs(std::vector<gunparts>&data, const size_t& pts_cat, const size_t& pot_cat, int& chang_t, const size_t& sel) {
			chang_max = (int)data.size() + 1;
			chang_t = std::clamp(chang_t, 0, chang_max - 1);
			if (chang_t == 0) { parts_p = nullptr; }
			else { parts_p = &data[chang_t - 1]; }
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
		//非必須品3
		void Set_chang_need3(std::vector<gunparts>&data1, std::vector<gunparts>&data2, const size_t& pts_cat, const size_t& pot_cat, int& chang_t) {
			chang_max = (int)(data1.size() + 1 + data2.size());
			chang_t = std::clamp(chang_t, 0, chang_max - 1);
			if (chang_t == 0) { parts_p = nullptr; }
			else {
				if (chang_t < data1.size() + 1) { parts_p = &data1[chang_t - 1]; }
				else { parts_p = &data2[chang_t - 1 - data1.size()]; }
			}
			if (chang_t != 0) {
				mine_ptr->add_parts(parts_p, pts_cat, port_ptr, pot_cat);
			}
			viewparts = port_ptr->obj.frame(port_ptr->rail_f[pot_cat][0].first);
		}
		template<class Y2, class D2>
		bool update(std::unique_ptr<Y2, D2>& MAPPTs, std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& DrawPts, std::unique_ptr<MAINLOOP, std::default_delete<MAINLOOP>>& MAINLOOPparts) {
			{
				bool changef = false;
				//演算
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
									mine_ptr->gun_stat[mine_ptr->base.thisparts->id].select = uint8_t(&s - &mine_ptr->base.thisparts->select[0]);
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
							port_cat = EnumAttachPoint::UNDER;
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
							port_cat = EnumAttachPoint::LEFTSIDE;
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
									else { parts_p = &MAINLOOPparts->get_mount_data()[chang - 1]; }
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
						//マズル
						{
							if (sel_p == sel_max) {
								port_ptr = &mine_ptr->base;
								port_cat = EnumAttachPoint::MAZZULE_;
								parts_cat = EnumGunParts::PARTS_MAZZULE;
								Set_chang_needs(MAINLOOPparts->get_mazzule_data(), parts_cat, port_cat, chang, 0);
							}
							sel_max++;
						}
						//ダストカバー
						{
							if (sel_p == sel_max) {
								port_ptr = &mine_ptr->base;
								port_cat = EnumAttachPoint::DUSTCOVER_BASE;
								parts_cat = EnumGunParts::PARTS_DUSTCOVER;
								Set_chang_needs(MAINLOOPparts->get_dustcover_data(), parts_cat, port_cat, chang, 0);
							}
							sel_max++;
						}
						//ストック
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
							port_cat = EnumAttachPoint::UPER;
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
								port_cat = EnumAttachPoint::UPER;
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
							//薬莢
							mine_ptr->calc_cart(rate);
							//エフェクト
							mine_ptr->calc_shot_effect();
						}
						//
						mine_ptr->gunf = true;
					}

					easing_set(&mine_ptr->gunanime_sel[1]->per, float(mine_ptr->gun_stat[mine_ptr->base.thisparts->id].select / std::max<size_t>(mine_ptr->base.thisparts->select.size() - 1, 1)), 0.5f);
					easing_set(&mine_ptr->gunanime_sel[0]->per, 1.f - mine_ptr->gunanime_sel[1]->per, 0.5f);

					mine_ptr->gunanime_shot->per = 1.f;
					if (mine_ptr->gunf) {
						mine_ptr->gunanime_shot->update(true, rate*2.f*mine_ptr->gunanime_shot->alltime / (FRAME_RATE / (600.f / 60.f)));
						if (mine_ptr->gunanime_shot->time == 0.f) {
							mine_ptr->gunf = false;
						}
					}
					//
					mine_ptr->base.obj.work_anime();

					//薬莢の処理
					for (auto& a : mine_ptr->cart) {
						a.get(MAPPTs, *mine_ptr);
					}
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
			//終了判定
			if (CheckHitKey(KEY_INPUT_ESCAPE) != 0) {
				return false;
			}
			if (Start.push()) {
				return false;
			}
			return true;
			//
		}
		void Dispose(std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& DrawPts) {
			DrawPts->Delete_Shadow();
		}
		//
		void UI_Draw(std::unique_ptr<MAINLOOP, std::default_delete<MAINLOOP>>& MAINLOOPparts) {

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
							font18.DrawString(100 + 250 * cnt, 400, p.mod.name, GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.name, GetColor(255, 255, 0));
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
							font18.DrawString(100 + 250 * cnt, 400, p.mod.name, GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.name, GetColor(255, 255, 0));
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
							font18.DrawString(100 + 250 * cnt, 400, p.mod.name, GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.name, GetColor(255, 255, 0));
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
							font18.DrawString(100 + 250 * cnt, 400, p.mod.name, GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.name, GetColor(255, 255, 0));
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
							font18.DrawString(100 + 250 * cnt, 400, p.mod.name, GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.name, GetColor(255, 255, 0));
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
							font18.DrawString(100 + 250 * cnt, 400, p.mod.name, GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.name, GetColor(255, 255, 0));
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
							font18.DrawString(100 + 250 * cnt, 400, p.mod.name, GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.name, GetColor(255, 255, 0));
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
							font18.DrawString(100 + 250 * cnt, 400, p.mod.name, GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.name, GetColor(255, 255, 0));
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
							font18.DrawString(100 + 250 * cnt, 400, p.mod.name, GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.name, GetColor(255, 255, 0));
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
							font18.DrawString(100 + 250 * cnt, 400, p.mod.name, GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.name, GetColor(255, 255, 0));
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
							font18.DrawString(100 + 250 * cnt, 400, p.mod.name, GetColor(255, 0, 0));
						}
						else {
							font18.DrawString(100 + 250 * cnt, 400, p.mod.name, GetColor(255, 255, 0));
						}
						cnt++;
					}
					break;
				}
				default:
					break;
				}
			}

			font18.DrawString(100, 525, "←→   :Parts Change", GetColor(255, 0, 0));
			font18.DrawString(100, 550, "↑↓   :Parts Select", GetColor(255, 0, 0));
			font18.DrawString(100, 575, "SPACE  :Go Battle", GetColor(255, 0, 0));
			font18.DrawString(100, 600, (Rot.on()) ? "RANGE  :FREE" : "RANGE  :FIXED", GetColor(255, 0, 0));
		}
		void BG_Draw() {
			DrawBox(0, 0, 1920, 1080, GetColor(128, 128, 128), TRUE);
		}
		void Shadow_Draw() {
			mine_ptr->Draw_gun();
			if (mine_ptr->magazine.attach) {
				mine_ptr->magazine.obj.DrawModel();
			}
		}
		void Main_Draw() {
			mine_ptr->Draw_gun();
			if (mine_ptr->magazine.attach) {
				mine_ptr->magazine.obj.DrawModel();
			}
		}
	};
	//
	class MAINLOOP {
		//カメラ
		cam_info camera_main;
		//データ
		MV1 body_obj, body_col;						//身体モデル
		MV1 hit_pic;								//弾痕
		GraphHandle light;							//ライトテクスチャ
		std::vector<gunparts> mazzule_data;			//GUNデータ
		std::vector<gunparts> grip_data;			//GUNデータ
		std::vector<gunparts> uperhandguard_data;	//GUNデータ
		std::vector<gunparts> underhandguard_data;	//GUNデータ
		std::vector<gunparts> stock_data;			//GUNデータ
		std::vector<gunparts> dustcover_data;		//GUNデータ
		std::vector<gunparts> sight_data;			//GUNデータ
		std::vector<gunparts> foregrip_data;		//GUNデータ
		std::vector<gunparts> light_data;			//GUNデータ
		std::vector<gunparts> laser_data;			//GUNデータ
		std::vector<gunparts> mount_data;			//GUNデータ
		std::vector<gunparts> gun_data;				//GUNデータ
		std::vector<gunparts> mag_data;				//GUNデータ
		std::vector<Meds> meds_data;				//GUNデータ
		//オブジェ
		std::vector<Chara> chara;					//キャラ
		std::vector<Items> item;					//アイテム
		std::vector<Hit> hit_obj;					//弾痕
		size_t hit_buf = 0;							//
		//
		std::unique_ptr<UIclass::UI, std::default_delete<UIclass::UI>> UIparts;
		std::unique_ptr<TPS_parts, std::default_delete<TPS_parts>> TPSparts;
		std::unique_ptr<rule, std::default_delete<rule>> ruleparts;
		std::unique_ptr<HostPassEffect, std::default_delete<HostPassEffect>> Hostpassparts_TPS;
		std::unique_ptr<MAPclass::MiniMap, std::default_delete<MAPclass::MiniMap>> minimapparts;
	public:
		std::vector<Chara>& get_chara() { return chara; }
		std::vector<Items>& get_item() { return item; }
		Chara& get_mine() { return chara[0]; }
		std::vector<gunparts>& get_mazzule_data() { return mazzule_data; }
		std::vector<gunparts>& get_grip_data() { return grip_data; }
		std::vector<gunparts>& get_uperhandguard_data() { return uperhandguard_data; }
		std::vector<gunparts>& get_underhandguard_data() { return underhandguard_data; }
		std::vector<gunparts>& get_stock_data() { return stock_data; }
		std::vector<gunparts>& get_dustcover_data() { return dustcover_data; }
		std::vector<gunparts>& get_sight_data() { return sight_data; }
		std::vector<gunparts>& get_foregrip_data() { return foregrip_data; }
		std::vector<gunparts>& get_light_data() { return light_data; }
		std::vector<gunparts>& get_laser_data() { return laser_data; }
		std::vector<gunparts>& get_mount_data() { return mount_data; }
		std::vector<gunparts>& get_gun_data() { return gun_data; }
		std::vector<gunparts>& get_mag_data() { return mag_data; }
		auto& get_camera() { return camera_main; }
		//
		MAINLOOP(std::unique_ptr<OPTION, std::default_delete<OPTION>>& OPTPTs) {
			//
			UIparts = std::make_unique<UIclass::UI>();
			//
			TPSparts = std::make_unique<TPS_parts>();
			//
			ruleparts = std::make_unique<rule>();
			//ホストパスエフェクト(フルスクリーン向け、TPS用)
			Hostpassparts_TPS = std::make_unique<HostPassEffect>(OPTPTs->DoF, OPTPTs->bloom, deskx, desky);
			//ミニマップ
			minimapparts = std::make_unique<MAPclass::MiniMap>();
			{
				//model
				light = GraphHandle::Load("data/light.png");					//ライト
				MV1::Load("data/model/body2/model.mv1", &this->body_obj, true);	//身体
				MV1::Load("data/model/body2/col.mv1", &this->body_col, true);	//身体col
				MV1::Load("data/model/hit/model.mv1", &this->hit_pic, true);	//弾痕
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
				//MAGデータ
				this->mag_data.resize(1);
				this->mag_data[0].mod.Ready("data/mag/", "AK_55_Magazine");
				//MEDデータ
				this->meds_data.resize(1);
				this->meds_data[0].mod.Ready("data/medkit/", "AIDkit");
			}
			//ロード画面1
			UIparts->load_window("アイテムオブジェクト");
			{
				//GUNデータ2
				std::for_each(this->mazzule_data.begin(), this->mazzule_data.end(), [&](gunparts& g) { g.Set_data_mazzule(&g - &this->mazzule_data[0]); });
				std::for_each(this->grip_data.begin(), this->grip_data.end(), [&](gunparts& g) { g.Set_data(&g - &this->grip_data[0]); });
				std::for_each(this->uperhandguard_data.begin(), this->uperhandguard_data.end(), [&](gunparts& g) { g.Set_data(&g - &this->uperhandguard_data[0]); });
				std::for_each(this->underhandguard_data.begin(), this->underhandguard_data.end(), [&](gunparts& g) { g.Set_data(&g - &this->underhandguard_data[0]); });
				std::for_each(this->mount_data.begin(), this->mount_data.end(), [&](gunparts& g) { g.Set_data(&g - &this->mount_data[0]); });
				std::for_each(this->stock_data.begin(), this->stock_data.end(), [&](gunparts& g) { g.Set_data(&g - &this->stock_data[0]); });
				std::for_each(this->dustcover_data.begin(), this->dustcover_data.end(), [&](gunparts& g) { g.Set_data(&g - &this->dustcover_data[0]); });
				std::for_each(this->light_data.begin(), this->light_data.end(), [&](gunparts& g) { g.Set_data(&g - &this->light_data[0]); });
				std::for_each(this->laser_data.begin(), this->laser_data.end(), [&](gunparts& g) { g.Set_data(&g - &this->laser_data[0]); });
				std::for_each(this->foregrip_data.begin(), this->foregrip_data.end(), [&](gunparts& g) { g.Set_data(&g - &this->foregrip_data[0]); });
				std::for_each(this->sight_data.begin(), this->sight_data.end(), [&](gunparts& g) { g.Set_data_sight(&g - &this->sight_data[0]); });
				std::for_each(this->mag_data.begin(), this->mag_data.end(), [&](gunparts& g) { g.Set_data_mag(&g - &this->mag_data[0]); });
				std::for_each(this->gun_data.begin(), this->gun_data.end(), [&](gunparts& g) { g.Set_data_base(&g - &this->gun_data[0]); });
				//MEDデータ2
				std::for_each(this->meds_data.begin(), this->meds_data.end(), [&](Meds& g) { g.Set_data(&g - &this->meds_data[0]); });
				//弾痕
				this->hit_obj.resize(24);
				this->hit_buf = 0;
				std::for_each(this->hit_obj.begin(), this->hit_obj.end(), [&](Hit& h) { h.Set(this->hit_pic); });
			}
			//ロード画面2
			UIparts->load_window("アイテムデータ");
		}
		template<class Y2, class D2>
		void first(std::unique_ptr<Y2, D2>& MAPPTs) {
			//マップ読み込み
			MAPPTs->Ready_map("data/map_new2");
			UIparts->load_window("マップ");
			MAPPTs->Set_map("data/maps/Set.txt", item, gun_data, mag_data, meds_data, "data/grassput.bmp");
			//キャラ設定
			chara.resize(MAPPTs->get_spawn_point().size());
			for (auto& c : chara) {
				c.Set(gun_data, 0, body_obj, body_col);
				//マガジン
				c.add_parts(&mag_data[0], EnumGunParts::PARTS_MAGAZINE);
			}
		}
		template<class Y2, class D2>
		void Set(std::unique_ptr<Y2, D2>& MAPPTs, std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& DrawPts, std::unique_ptr<OPTION, std::default_delete<OPTION>>& OPTPTs) {
			for (auto& c : chara) {
				//カスタムattach
				if (&c != &get_mine()) {
					//grip
					c.add_parts(&grip_data[0], EnumGunParts::PARTS_GRIP, &c.base, EnumAttachPoint::GRIP_BASE);
					//uperhandguard
					c.add_parts(&uperhandguard_data[0], EnumGunParts::PARTS_UPER_HGUARD, &c.base, EnumAttachPoint::UPER_HANDGUARD);
					{
						c.add_parts(&sight_data[0], EnumGunParts::PARTS_SIGHT, &c.uperhandguard, EnumAttachPoint::UPER);
					}
					//underhandguard
					c.add_parts(&underhandguard_data[0], EnumGunParts::PARTS_UNDER_HGUARD, &c.base, EnumAttachPoint::UNDER_HANDGUARD);
					{
						c.add_parts(&foregrip_data[0], EnumGunParts::PARTS_FOREGRIP, &c.underhandguard, EnumAttachPoint::UNDER);
						c.add_parts(&light_data[0], EnumGunParts::PARTS_LIGHT, &c.underhandguard, EnumAttachPoint::LEFTSIDE);
						c.add_parts(&laser_data[0], EnumGunParts::PARTS_LASER, &c.underhandguard, EnumAttachPoint::RIGHTSIDE);
					}
					//sidemount
					c.add_parts(&mount_data[0], EnumGunParts::PARTS_MOUNT, &c.base, EnumAttachPoint::SIDEMOUNT_BASE);
					{
						c.add_parts(&mount_data[1], EnumGunParts::PARTS_MOUNT, &c.mount_[0], EnumAttachPoint::SIDEMOUNT);
						{
							c.add_parts(&sight_data[1], EnumGunParts::PARTS_SIGHT, &c.mount_[1], EnumAttachPoint::UPER);
						}
					}
					//optional
					c.add_parts(&mazzule_data[0], EnumGunParts::PARTS_MAZZULE, &c.base, EnumAttachPoint::MAZZULE_);
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
					auto i = &c - &chara[0];
					c.spawn(MAPPTs->get_spawn_point()[i], MATRIX_ref::RotY(atan2f(MAPPTs->get_spawn_point()[i].x(), MAPPTs->get_spawn_point()[i].z())));
				}
			}
			//ライティング
			DrawPts->Set_Light_Shadow(MAPPTs->map_col_get().mesh_maxpos(0), MAPPTs->map_col_get().mesh_minpos(0), VGet(0.5f, -0.5f, 0.5f), [&] { MAPPTs->Shadow_Draw(); });
			SetGlobalAmbientLight(GetColorF(0.42f, 0.41f, 0.40f, 0.0f));
			//
			MAPPTs->Start_map();															//環境
			std::for_each(chara.begin(), chara.end(), [&](Chara& c) { c.Start(MAPPTs); });//プレイヤー操作変数群
			SetMousePoint(deskx / 2, desky / 2);											//
			camera_main.set_cam_info(deg2rad(DrawPts->use_vr ? 90 : OPTPTs->fov_pc), 0.1f, 200.f);//1P
			TPSparts->Set(OPTPTs->fov_pc);															//TPS
			ruleparts->Set();																//ルール
		}
		template<class Y2, class D2>
		bool update(std::unique_ptr<Y2, D2>& MAPPTs, std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& DrawPts, std::unique_ptr<OPTION, std::default_delete<OPTION>>& OPTPTs) {
			{
				//演算
				{
					//座標取得
					if (DrawPts->tracker_num.size() > 0) {
						auto& ptr_ = (*DrawPts->get_device())[DrawPts->tracker_num[0]];
						DrawPts->GetDevicePositionVR(DrawPts->tracker_num[0], &get_mine().pos_WAIST, &get_mine().mat_WAIST);
						if (get_mine().Start_c || (ptr_.turn && ptr_.now) != get_mine().oldv_1_2) {
							get_mine().pos_WAIST_rep = VGet(get_mine().pos_WAIST.x(), 0.f, get_mine().pos_WAIST.z());
						}
						get_mine().oldv_1_2 = ptr_.turn && ptr_.now;
						get_mine().pos_WAIST = get_mine().pos_WAIST - get_mine().pos_WAIST_rep;
					}
					for (auto& c : chara) {
						c.Set_alive(item, MAPPTs);
					}

					//プレイヤー操作
					{
						for (auto& c : chara) {
							if (&c == &get_mine()) {
								//mine
								if (DrawPts->use_vr) {
									c.operation_VR(DrawPts, !ruleparts->getStart() || ruleparts->getend());
								}
								else {
									c.operation(!ruleparts->getStart() || ruleparts->getend(), camera_main.fov / deg2rad(DrawPts->use_vr ? 90 : OPTPTs->fov_pc));
								}
							}
							else {
								//cpu
								c.operation_NPC(MAPPTs, chara, !ruleparts->getStart() || ruleparts->getend());
							}
							//common
							c.operation_2();
						}
					}
					//
					for (auto& c : chara) {
						//壁その他の判定
						c.wall_col(MAPPTs, get_mine(), DrawPts->use_vr);
						//body演算
						if (DrawPts->use_vr && (&c == &get_mine())) {
							c.move_vr(MAPPTs, DrawPts);
						}
						else {
							c.move_other(MAPPTs, get_mine());
						}
						c.body.work_anime();
						//col演算
						c.frame_s.copy_frame(c.body, c.colframe_, &c.col);
						c.col.work_anime();
						c.col.RefreshCollInfo(-1, 0);
						c.col.RefreshCollInfo(-1, 1);
						c.col.RefreshCollInfo(-1, 2);
						//
						if (!(DrawPts->use_vr && (&c == &get_mine()))) {
							//視点取得
							c.pos_HMD = (c.body.frame(c.frame_s.RIGHTeye_f.first) + (c.body.frame(c.frame_s.LEFTeye_f.first) - c.body.frame(c.frame_s.RIGHTeye_f.first))*0.5f) - c.pos;
							//銃器
							c.calc_gun();
						}
						//射撃関連
						c.calc_shot(MAPPTs, DrawPts, chara, hit_obj, hit_buf, meds_data, item);
					}
					//アイテム演算
					for (auto& g : item) {
						g.update(item, MAPPTs);
					}
					//アイテム拾う
					for (auto& c : chara) {
						c.canget_gunitem = false;
						c.canget_magitem = false;
						c.canget_meditem = false;
						for (auto& g : item) {
							g.Get_item_2(c, MAPPTs);
						}
					}
					//空マガジンを削除する
					while (true) {
						bool demag_flag = false;
						size_t demag_id = 0;
						for (auto& i : item) {
							if (i.ptr_mag != nullptr && i.del_timer >= 5.f) {
								i.Delete_item();
								demag_id = &i - &item[0];
								demag_flag = true;
								break;
							}
						}
						if (demag_flag) {
							item.erase(item.begin() + demag_id);
						}
						else {
							break;
						}
					}
					//物理演算、アニメーション
					for (auto& c : chara) {
						if (c.Start_c) {
							c.body.PhysicsResetState();
							c.Start_c = false;
						}
						else  if (c.Start_b) {
							c.body.PhysicsResetState();
							c.Start_b = false;
						}
						else {
							c.body.PhysicsCalculation(1000.f / GetFPS());
						}
					}
					//
					for (auto&s : get_mine().sight_) {
						s.Set_reticle();
					}
				}
				//campos,camvec,camupの指定
				{
					auto& tgt = get_mine();
					tgt.Set_cam(MAPPTs, camera_main, chara, deg2rad(DrawPts->use_vr ? 90 : OPTPTs->fov_pc), DrawPts->use_vr);
				}
				//ルール保存
				UIparts->Set_rule(ruleparts->getReady(), ruleparts->gettimer());
				ruleparts->update();
				//ライト
				for (auto& c : chara) {
					if (&c == &get_mine()) {
						c.Set_Light();
					}
				}
				TPSparts->key_TPS.get_in(CheckHitKey(KEY_INPUT_LCONTROL) != 0);
				//ミニマップ
				{
					if (TPSparts->key_TPS.on()) {
						minimapparts->Set(chara, chara[TPSparts->sel_cam], MAPPTs);
					}
					else {
						minimapparts->Set(chara, get_mine(), MAPPTs);
					}
				}
				//TPS描画
				TPSparts->Set_info(MAPPTs, chara);
				if (TPSparts->key_TPS.on()) {
					//影用意
					DrawPts->Ready_Shadow(TPSparts->camera_TPS.campos, [&] {Shadow_Draw(); }, VGet(2.f, 2.5f, 2.f), VGet(5.f, 2.5f, 5.f));
					//書き込み
					{
						Hostpassparts_TPS->BUF_draw([&]() { BG_Draw(MAPPTs); }, [&] {DrawPts->Draw_by_Shadow([&] {Main_Draw(MAPPTs); }); }, TPSparts->camera_TPS);	//被写体深度描画
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
						for (auto& c : chara) {
							c.check_CameraViewClip(MAPPTs);
						}
					}
				}
			}
			//終了判定
			if (CheckHitKey(KEY_INPUT_ESCAPE) != 0) {
				return false;
			}
			if (CheckHitKey(KEY_INPUT_O) != 0) {
				return false;
			}
			return true;
			//
		}
		template<class Y2, class D2>
		void Dispose(std::unique_ptr<Y2, D2>& MAPPTs, std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& DrawPts) {
			for (auto& c : chara) {
				c.Delete_chara();
			}
			chara.clear();
			for (auto& i : item) {
				i.Delete_item();
			}
			item.clear();
			MAPPTs->Delete_map();
			DrawPts->Delete_Shadow();
		}
		//
		void UI_Draw(std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& DrawPts) {
			UIparts->Set_Draw(get_mine(), DrawPts->use_vr);
		}
		template<class Y2, class D2>
		void BG_Draw(std::unique_ptr<Y2, D2>& MAPPTs) {
			MAPPTs->sky_Draw();
		}
		void Shadow_Draw() {
			for (auto& c : chara) { c.Draw_chara(); }
			for (auto& g : item) { g.Draw_item(); }
		}
		template<class Y2, class D2>
		void Main_Draw(std::unique_ptr<Y2, D2>& MAPPTs) {
			//map
			MAPPTs->main_Draw();
			//命中痕
			std::for_each(hit_obj.begin(), hit_obj.end(), [&](Hit& s) { s.Draw(); });
			//サイト
			for (auto& s : get_mine().sight_) {
				s.Draw_reticle(MAPPTs);
			}
			//キャラ
			std::for_each(chara.begin(), chara.end(), [&](Chara& c) { c.Draw_chara(); });
			//レーザー
			std::for_each(chara.begin(), chara.end(), [&](Chara& c) { c.Draw_laser(chara, MAPPTs, light); });
			//アイテム
			std::for_each(item.begin(), item.end(), [&](Items& i) { i.Draw_item(get_mine()); });
			//銃弾
			SetFogEnable(FALSE);
			SetUseLighting(FALSE);
			std::for_each(chara.begin(), chara.end(), [&](Chara& c) { c.Draw_ammo(); });
			SetUseLighting(TRUE);
			SetFogEnable(TRUE);
			//
		}
		void Item_Draw(std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& DrawPts) {
			for (auto& c : chara) {
				for (auto& a : c.bullet) {
					a.Set_Draw();
				}
			}
			UIparts->item_Draw(chara, get_mine(), get_item(), camera_main.campos, DrawPts->use_vr);
		}
		void LAST_Draw() {
			if (TPSparts->key_TPS.on()) {
				Hostpassparts_TPS->get_main().DrawGraph(0, 0, true);
			}
			//minimap
			minimapparts->Draw(64, 64);
		}
	};
};