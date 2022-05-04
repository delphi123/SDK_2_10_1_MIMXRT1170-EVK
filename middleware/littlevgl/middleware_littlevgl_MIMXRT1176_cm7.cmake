include_guard(GLOBAL)
message("middleware_littlevgl component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_core/lv_disp.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_core/lv_group.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_core/lv_indev.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_core/lv_obj.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_core/lv_refr.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_core/lv_style.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_draw/lv_draw_arc.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_draw/lv_draw_blend.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_draw/lv_draw_img.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_draw/lv_draw_label.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_draw/lv_draw_line.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_draw/lv_draw_mask.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_draw/lv_draw_rect.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_draw/lv_draw_triangle.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_draw/lv_img_buf.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_draw/lv_img_cache.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_draw/lv_img_decoder.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_font/lv_font.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_font/lv_font_dejavu_16_persian_hebrew.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_font/lv_font_fmt_txt.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_font/lv_font_loader.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_font/lv_font_montserrat_8.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_font/lv_font_montserrat_10.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_font/lv_font_montserrat_12.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_font/lv_font_montserrat_12_subpx.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_font/lv_font_montserrat_14.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_font/lv_font_montserrat_16.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_font/lv_font_montserrat_18.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_font/lv_font_montserrat_20.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_font/lv_font_montserrat_22.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_font/lv_font_montserrat_24.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_font/lv_font_montserrat_26.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_font/lv_font_montserrat_28.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_font/lv_font_montserrat_28_compressed.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_font/lv_font_montserrat_30.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_font/lv_font_montserrat_32.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_font/lv_font_montserrat_34.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_font/lv_font_montserrat_36.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_font/lv_font_montserrat_38.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_font/lv_font_montserrat_40.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_font/lv_font_montserrat_42.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_font/lv_font_montserrat_44.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_font/lv_font_montserrat_46.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_font/lv_font_montserrat_48.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_font/lv_font_simsun_16_cjk.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_font/lv_font_unscii_8.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_font/lv_font_unscii_16.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_gpu/lv_gpu_nxp_pxp.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_gpu/lv_gpu_nxp_pxp_osa.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_gpu/lv_gpu_nxp_vglite.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_hal/lv_hal_disp.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_hal/lv_hal_indev.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_hal/lv_hal_tick.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_misc/lv_anim.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_misc/lv_area.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_misc/lv_async.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_misc/lv_bidi.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_misc/lv_color.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_misc/lv_debug.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_misc/lv_fs.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_misc/lv_gc.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_misc/lv_ll.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_misc/lv_log.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_misc/lv_math.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_misc/lv_mem.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_misc/lv_printf.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_misc/lv_task.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_misc/lv_templ.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_misc/lv_txt.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_misc/lv_txt_ap.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_misc/lv_utils.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_themes/lv_theme.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_themes/lv_theme_empty.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_themes/lv_theme_material.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_themes/lv_theme_mono.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_themes/lv_theme_template.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_anim_img.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_arc.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_bar.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_btn.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_btnmatrix.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_calendar.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_canvas.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_chart.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_checkbox.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_cont.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_cpicker.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_dropdown.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_gauge.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_img.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_imgbtn.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_keyboard.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_label.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_led.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_line.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_linemeter.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_list.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_msgbox.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_objmask.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_objx_templ.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_page.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_roller.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_slider.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_spinbox.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_spinner.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_switch.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_table.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_tabview.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_textarea.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_tileview.c
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src/lv_widgets/lv_win.c
)


target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/lvgl
    ${CMAKE_CURRENT_LIST_DIR}/lvgl/src
    ${CMAKE_CURRENT_LIST_DIR}/.
)


