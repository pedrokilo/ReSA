"""Prueba negativa del banco LIB3A: copia de libs/libjpeg-6b con UNA mutacion por funcion, activada por la variable global lib3a_mut (la define bench.exe; LIB3A_MUT=k).
La fuente buena nunca se toca. Dos rondas de mutaciones distintas. Uso: python mutate.py <ronda 1|2> <directorio de salida>"""
import sys, os, shutil, re
J = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'libs', 'libjpeg-6b')
C = 'jccolor'; D = 'jcdctmgr'; U = 'jdsample'; K = 'jdcolor'
# (k, nombre en el banco, archivo, funcion, texto original (primera aparicion dentro de la funcion), texto nuevo)
R1 = [
 (1, 'rgb_ycc_start', C, 'rgb_ycc_start', 'FIX(0.11400) * i     + ONE_HALF;', 'FIX(0.11400) * i     + ONE_HALF + ((lib3a_mut==1 && i==255)?1:0);'),
 (2, 'rgb_ycc_convert', C, 'rgb_ycc_convert', 'ctab[b+B_CB_OFF])', 'ctab[b+B_CB_OFF] + ((lib3a_mut==2 && b==77 && col==3)?65536:0))'),
 (3, 'rgb_gray_convert', C, 'rgb_gray_convert', 'ctab[b+B_Y_OFF])', 'ctab[b+B_Y_OFF] + ((lib3a_mut==3 && r==100 && g==100)?65536:0))'),
 (4, 'cmyk_ycck_convert', C, 'cmyk_ycck_convert', 'outptr3[col] = inptr[3];', 'outptr3[col] = inptr[3] ^ ((lib3a_mut==4 && col==9)?1:0);'),
 (5, 'grayscale_convert', C, 'grayscale_convert', 'outptr[col] = inptr[0];', 'outptr[col] = inptr[0] + ((lib3a_mut==5 && inptr[0]==77)?1:0);'),
 (6, 'null_convert', C, 'null_convert', 'outptr[col] = inptr[ci];', 'outptr[col] = inptr[ci] + ((lib3a_mut==6 && ci==2 && col==4)?1:0);'),
 (7, 'null_method', C, 'null_method', '/* no work needed */', 'if (lib3a_mut==7) cinfo->image_width++;'),
 (8, 'start_pass_fdctmgr', D, 'start_pass_fdctmgr', '8.0)));', '((lib3a_mut==8 && i==37)?8.5:8.0))));'),
 (9, 'jpeg_fdct_float', 'jfdctflt', 'jpeg_fdct_float', '((FAST_FLOAT) 0.382683433)', '((FAST_FLOAT) (lib3a_mut==9 && ctr==3 ? 0.3827 : 0.382683433))'),
 (10, 'forward_DCT_float', D, 'forward_DCT_float', '- 16384);', '- 16384 + ((lib3a_mut==10 && i==37)?1:0));'),
 (11, 'start_pass_upsample', U, 'start_pass_upsample', 'upsample->rows_to_go = cinfo->output_height;', 'upsample->rows_to_go = cinfo->output_height + (lib3a_mut==11 ? 1 : 0);'),
 (12, 'fullsize_upsample', U, 'fullsize_upsample', '*output_data_ptr = input_data;', '*output_data_ptr = (lib3a_mut==12 && cinfo->max_v_samp_factor==3) ? input_data + 1 : input_data;'),
 (13, 'noop_upsample', U, 'noop_upsample', '*output_data_ptr = NULL;', '*output_data_ptr = (lib3a_mut==13 && compptr->component_index==3) ? input_data : NULL;'),
 (14, 'int_upsample', U, 'int_upsample', 'invalue = *inptr++;', 'invalue = *inptr++; if (lib3a_mut==14 && invalue==200) invalue = 201;'),
 (15, 'h2v1_upsample', U, 'h2v1_upsample', 'invalue = *inptr++;', 'invalue = *inptr++; if (lib3a_mut==15 && invalue==100) invalue = 101;'),
 (16, 'h2v2_upsample', U, 'h2v2_upsample', 'invalue = *inptr++;', 'invalue = *inptr++; if (lib3a_mut==16 && invalue==50) invalue = 51;'),
 (17, 'h2v1_fancy_upsample', U, 'h2v1_fancy_upsample', 'GETJSAMPLE(*inptr) + 2) >> 2);', 'GETJSAMPLE(*inptr) + 2 + ((lib3a_mut==17 && invalue==100)?1:0)) >> 2);'),
 (18, 'h2v2_fancy_upsample', U, 'h2v2_fancy_upsample', '(thiscolsum * 4 + 7) >> 4);', '(thiscolsum * 4 + 7 + ((lib3a_mut==18 && thiscolsum==400)?9:0)) >> 4);'),
 (19, 'ycc_rgb_convert', K, 'ycc_rgb_convert', 'range_limit[y + Cbbtab[cb]];', 'range_limit[y + Cbbtab[cb] + ((lib3a_mut==19 && y==10)?1:0)];'),
 (20, 'dc_grayscale_convert', K, 'grayscale_convert', 'jcopy_sample_rows(input_buf[0],', 'if (lib3a_mut==20 && num_rows==3) num_rows--;\n  jcopy_sample_rows(input_buf[0],'),
 (21, 'gray_rgb_convert', K, 'gray_rgb_convert', '= inptr[col];', '= inptr[col] + ((lib3a_mut==21 && col==2)?1:0);'),
 (22, 'ycck_cmyk_convert', K, 'ycck_cmyk_convert', 'outptr[3] = inptr3[col];', 'outptr[3] = inptr3[col] + ((lib3a_mut==22 && inptr3[col]==255)?1:0);'),
]
R2 = [
 (1, 'rgb_ycc_start', C, 'rgb_ycc_start', '+ ONE_HALF-1;', '+ ONE_HALF-1 + ((lib3a_mut==1 && i==0)?1:0);'),
 (2, 'rgb_ycc_convert', C, 'rgb_ycc_convert', 'r = GETJSAMPLE(inptr[RGB_RED]);', 'r = GETJSAMPLE(inptr[RGB_RED]) ^ ((lib3a_mut==2 && col==63)?1:0);'),
 (3, 'rgb_gray_convert', C, 'rgb_gray_convert', 'g = GETJSAMPLE(inptr[RGB_GREEN]);', 'g = GETJSAMPLE(inptr[RGB_GREEN]) ^ ((lib3a_mut==3 && col==70)?2:0);'),
 (4, 'cmyk_ycck_convert', C, 'cmyk_ycck_convert', 'r = MAXJSAMPLE - GETJSAMPLE(inptr[0]);', 'r = MAXJSAMPLE - GETJSAMPLE(inptr[0]) - ((lib3a_mut==4 && col==33 && inptr[0]!=255)?1:0);'),
 (5, 'grayscale_convert', C, 'grayscale_convert', 'output_row++;', 'output_row += (lib3a_mut==5 && num_rows==3) ? 0 : 1;'),
 (6, 'null_convert', C, 'null_convert', 'inptr += nc;', 'inptr += nc + ((lib3a_mut==6 && col==7 && nc==4)?1:0);'),
 (7, 'null_method', C, 'null_method', '/* no work needed */', 'if (lib3a_mut==7) cinfo->num_components ^= 1;'),
 (8, 'start_pass_fdctmgr', D, 'start_pass_fdctmgr', '(double) qtbl->quantval[i] *', '(double) qtbl->quantval[i] * ((lib3a_mut==8 && i==5 && qtblno==2) ? 1.001 : 1.0) *'),
 (9, 'jpeg_fdct_float', 'jfdctflt', 'jpeg_fdct_float', '((FAST_FLOAT) 0.707106781)', '((FAST_FLOAT) (lib3a_mut==9 && ctr==5 ? 0.7072 : 0.707106781))'),
 (10, 'forward_DCT_float', D, 'forward_DCT_float', 'temp = workspace[i] * divisors[i];', 'temp = workspace[i] * divisors[i] + ((lib3a_mut==10 && i==63)?0.7f:0.0f);'),
 (11, 'start_pass_upsample', U, 'start_pass_upsample', 'upsample->next_row_out = cinfo->max_v_samp_factor;', 'upsample->next_row_out = cinfo->max_v_samp_factor - ((lib3a_mut==11 && cinfo->max_v_samp_factor==4)?1:0);'),
 (12, 'fullsize_upsample', U, 'fullsize_upsample', '*output_data_ptr = input_data;', '*output_data_ptr = ((lib3a_mut==12 && compptr->component_index==1) ? input_data - 1 : input_data);'),
 (13, 'noop_upsample', U, 'noop_upsample', '*output_data_ptr = NULL;', '*output_data_ptr = NULL; if (lib3a_mut==13 && cinfo->output_width==33) *output_data_ptr = input_data;'),
 (14, 'int_upsample', U, 'int_upsample', 'for (h = h_expand; h > 0; h--) {', 'for (h = h_expand - ((lib3a_mut==14 && h_expand==4 && v_expand==2)?1:0); h > 0; h--) {'),
 (15, 'h2v1_upsample', U, 'h2v1_upsample', '*outptr++ = invalue;\n      *outptr++ = invalue;', '*outptr++ = invalue;\n      *outptr++ = invalue ^ ((lib3a_mut==15 && invalue==128)?1:0);'),
 (16, 'h2v2_upsample', U, 'h2v2_upsample', '*outptr++ = invalue;\n      *outptr++ = invalue;', '*outptr++ = invalue;\n      *outptr++ = invalue ^ ((lib3a_mut==16 && invalue==127)?1:0);'),
 (17, 'h2v1_fancy_upsample', U, 'h2v1_fancy_upsample', 'GETJSAMPLE(inptr[-1]) + 1) >> 2);', 'GETJSAMPLE(inptr[-1]) + 1 + ((lib3a_mut==17 && invalue==255)?2:0)) >> 2);'),
 (18, 'h2v2_fancy_upsample', U, 'h2v2_fancy_upsample', '(thiscolsum * 4 + 8) >> 4);', '(thiscolsum * 4 + 8 + ((lib3a_mut==18 && thiscolsum==1020)?8:0)) >> 4);'),
 (19, 'ycc_rgb_convert', K, 'ycc_rgb_convert', 'range_limit[y + Crrtab[cr]];', 'range_limit[y + Crrtab[cr] - ((lib3a_mut==19 && cr==128 && y>0)?1:0)];'),
 (20, 'dc_grayscale_convert', K, 'grayscale_convert', '(int) input_row, output_buf, 0,', '(int) input_row - ((lib3a_mut==20 && num_rows==2 && input_row>0)?1:0), output_buf, 0,'),
 (21, 'gray_rgb_convert', K, 'gray_rgb_convert', 'outptr[RGB_RED] = outptr[RGB_GREEN] = outptr[RGB_BLUE] = inptr[col];', 'outptr[RGB_RED] = outptr[RGB_GREEN] = inptr[col]; outptr[RGB_BLUE] = inptr[col] ^ ((lib3a_mut==21 && col==11)?1:0);'),
 (22, 'ycck_cmyk_convert', K, 'ycck_cmyk_convert', 'outptr[0] = range_limit[MAXJSAMPLE - (y + Crrtab[cr])];', 'outptr[0] = range_limit[MAXJSAMPLE - (y + Crrtab[cr] + ((lib3a_mut==22 && cr==0)?1:0))];'),
]
R1 += [
 (23, 'dc_null_convert', K, 'null_convert', '*outptr = *inptr++;', '*outptr = *inptr++ + ((lib3a_mut==23 && ci==1 && count==3)?1:0);'),
 (24, 'start_pass_dcolor', K, 'start_pass_dcolor', '/* no work needed */', 'if (lib3a_mut==24) cinfo->output_width++;'),
]
R2 += [
 (23, 'dc_null_convert', K, 'null_convert', 'outptr += num_components;', 'outptr += num_components - ((lib3a_mut==23 && ci==2 && count==7)?1:0);'),
 (24, 'start_pass_dcolor', K, 'start_pass_dcolor', '/* no work needed */', 'if (lib3a_mut==24) cinfo->num_components ^= 2;'),
]
# ---- LIB3B
R1 += [
 (25, 'jpeg_idct_float', 'jidctflt', 'jpeg_idct_float', 'wsptr[DCTSIZE*5] = tmp2 - tmp5;', 'wsptr[DCTSIZE*5] = tmp2 - tmp5 - ((lib3a_mut==25 && ctr==3)?8.0f:0.0f);'),
]
R2 += [
 (25, 'jpeg_idct_float', 'jidctflt', 'jpeg_idct_float', 'tmp1 = tmp11 + tmp12;', 'tmp1 = tmp11 + tmp12 + ((lib3a_mut==25 && ctr==6)?40.0f:0.0f);'),
]
R1 += [
 (26, 'emit_message', 'jerror', 'emit_message', 'err->trace_level >= 3)', 'err->trace_level >= 3 + (lib3a_mut==26 ? 1 : 0))'),
 (27, 'reset_error_mgr', 'jerror', 'reset_error_mgr', 'num_warnings = 0;', 'num_warnings = (lib3a_mut==27 && cinfo->err->msg_code < 0) ? 1 : 0;'),
 (28, 'format_message', 'jerror', 'format_message', 'err->msg_parm.i[1],', 'err->msg_parm.i[1] + (lib3a_mut==28 ? 1 : 0),'),
 (29, 'finish_output_pass', 'jdmaster', 'finish_output_pass', 'master->pass_number++;', 'master->pass_number += (lib3a_mut==29 && cinfo->quantize_colors) ? 2 : 1;'),
 (30, 'reset_marker_reader', 'jdmarker', 'reset_marker_reader', 'marker->pub.discarded_bytes = 0;', 'marker->pub.discarded_bytes = (lib3a_mut==30 && (cinfo->image_width & 15) == 5) ? 1 : 0;'),
 (31, 'finish_input_pass', 'jdinput', 'finish_input_pass', 'cinfo->inputctl->consume_input = consume_markers;', 'cinfo->inputctl->consume_input = (lib3a_mut==31 && (cinfo->image_width & 7) == 3) ? NULL : consume_markers;'),
 (32, 'start_pass_dpost', 'jdpostct', 'start_pass_dpost', 'post->starting_row = post->next_row = 0;', 'post->starting_row = post->next_row = 0; if (lib3a_mut==32 && cinfo->quantize_colors && (cinfo->image_width & 3) == 1) post->next_row = 1;'),
 (33, 'start_pass_main_d', 'jdmainct', 'start_pass_main', 'main->buffer_full = FALSE;', 'main->buffer_full = (lib3a_mut==33 && cinfo->num_components == 3) ? TRUE : FALSE;'),
 (34, 'start_input_pass_coef', 'jdcoefct', 'start_input_pass', 'cinfo->input_iMCU_row = 0;', 'cinfo->input_iMCU_row = (lib3a_mut==34 && cinfo->total_iMCU_rows == 3) ? 1 : 0;'),
 (35, 'start_output_pass_coef', 'jdcoefct', 'start_output_pass', 'cinfo->output_iMCU_row = 0;', 'cinfo->output_iMCU_row = (lib3a_mut==35 && (cinfo->output_width & 7) == 3) ? 1 : 0;'),
 (36, 'start_pass_main_c', 'jcmainct', 'start_pass_main', 'main->cur_iMCU_row = 0;', 'main->cur_iMCU_row = (lib3a_mut==36 && cinfo->image_width % 5 == 2) ? 1 : 0;'),
 (37, 'start_pass_coef_c', 'jccoefct', 'start_pass_coef', 'coef->iMCU_row_num = 0;', 'coef->iMCU_row_num = (lib3a_mut==37 && cinfo->image_width % 7 == 2) ? 1 : 0;'),
 (38, 'start_pass_prep', 'jcprepct', 'start_pass_prep', 'prep->next_buf_row = 0;', 'prep->next_buf_row = (lib3a_mut==38 && cinfo->image_width % 5 == 3) ? 1 : 0;'),
 (39, 'finish_pass_master', 'jcmaster', 'finish_pass_master', 'master->pass_number++;', 'master->pass_number += (lib3a_mut==39 && cinfo->optimize_coding && cinfo->image_width % 4 == 1) ? 2 : 1;'),
 (40, 'emit_byte', 'jcmarker', 'emit_byte', '(JOCTET) val;', '(JOCTET) val ^ ((lib3a_mut==40 && val == 77) ? 1 : 0);'),
 (41, 'write_marker_header', 'jcmarker', 'write_marker_header', 'if (datalen > (unsigned int) 65533)', 'if (datalen > (unsigned int) 65533 - ((lib3a_mut==41 && datalen == 65533) ? 1 : 0))'),
]
R2 += [
 (26, 'emit_message', 'jerror', 'emit_message', 'err->num_warnings++;', 'err->num_warnings += (lib3a_mut==26 && err->trace_level == 5) ? 2 : 1;'),
 (27, 'reset_error_mgr', 'jerror', 'reset_error_mgr', 'msg_code = 0;', 'msg_code = (lib3a_mut==27 && (cinfo->err->msg_code & 255) == 51) ? 1 : 0;'),
 (28, 'format_message', 'jerror', 'format_message', 'msg_code <= err->last_addon_message)', 'msg_code <= err->last_addon_message - (lib3a_mut==28 ? 1 : 0))'),
 (29, 'finish_output_pass', 'jdmaster', 'finish_output_pass', '(*cinfo->cquantize->finish_pass) (cinfo);', 'if (!(lib3a_mut==29 && (master->pass_number & 7) == 3)) (*cinfo->cquantize->finish_pass) (cinfo);'),
 (30, 'reset_marker_reader', 'jdmarker', 'reset_marker_reader', 'marker->cur_marker = NULL;', 'marker->cur_marker = (lib3a_mut==30 && (cinfo->image_height & 7) == 2) ? (jpeg_saved_marker_ptr) 1 : NULL;'),
 (31, 'finish_input_pass', 'jdinput', 'finish_input_pass', 'cinfo->inputctl->consume_input = consume_markers;', 'cinfo->inputctl->consume_input = (lib3a_mut==31 && (cinfo->image_height & 3) == 1) ? (int (*) (j_decompress_ptr)) finish_input_pass : consume_markers;'),
 (32, 'start_pass_dpost', 'jdpostct', 'start_pass_dpost', 'post->pub.post_process_data = cinfo->upsample->upsample;', 'post->pub.post_process_data = (lib3a_mut==32 && (cinfo->image_height & 7) == 5) ? NULL : cinfo->upsample->upsample;'),
 (33, 'start_pass_main_d', 'jdmainct', 'start_pass_main', 'main->iMCU_row_ctr = 0;', 'main->iMCU_row_ctr = (lib3a_mut==33 && cinfo->min_DCT_scaled_size == 8) ? 1 : 0;'),
 (34, 'start_input_pass_coef', 'jdcoefct', 'start_input_pass', 'start_iMCU_row(cinfo);', 'start_iMCU_row(cinfo); if (lib3a_mut==34 && cinfo->comps_in_scan == 2) ((my_coef_ptr) cinfo->coef)->MCU_ctr = 1;'),
 (35, 'start_output_pass_coef', 'jdcoefct', 'start_output_pass', 'cinfo->output_iMCU_row = 0;', 'cinfo->output_iMCU_row = (lib3a_mut==35 && (cinfo->output_height & 3) == 2) ? 2 : 0;'),
 (36, 'start_pass_main_c', 'jcmainct', 'start_pass_main', 'main->suspended = FALSE;', 'main->suspended = (lib3a_mut==36 && cinfo->image_height % 5 == 1) ? TRUE : FALSE;'),
 (37, 'start_pass_coef_c', 'jccoefct', 'start_pass_coef', 'coef->pub.compress_data = compress_output;', 'coef->pub.compress_data = (lib3a_mut==37 && cinfo->image_width % 3 == 1) ? compress_first_pass : compress_output;'),
 (38, 'start_pass_prep', 'jcprepct', 'start_pass_prep', 'prep->rows_to_go = cinfo->image_height;', 'prep->rows_to_go = cinfo->image_height + ((lib3a_mut==38 && cinfo->image_height % 5 == 2) ? 1 : 0);'),
 (39, 'finish_pass_master', 'jcmaster', 'finish_pass_master', 'master->pass_type = output_pass;', 'master->pass_type = (lib3a_mut==39 && cinfo->image_width % 3 == 0) ? huff_opt_pass : output_pass;'),
 (40, 'emit_byte', 'jcmarker', 'emit_byte', '--dest->free_in_buffer == 0', '--dest->free_in_buffer == ((lib3a_mut==40 && cinfo->image_width % 4 == 1) ? 1 : 0)'),
 (41, 'write_marker_header', 'jcmarker', 'write_marker_header', '(int) (datalen + 2)', '(int) (datalen + 2 + ((lib3a_mut==41 && (datalen & 255) == 77) ? 1 : 0))'),
]
RONDAS = {1: R1, 2: R2}

def main():
    ronda, out = int(sys.argv[1]), sys.argv[2]
    if os.path.exists(out): shutil.rmtree(out)
    shutil.copytree(J, out)
    with open(os.path.join(out, 'exe_config.h'), 'a', newline='\n') as f: f.write('\nextern int lib3a_mut; /* solo copias mutadas */\n')
    for k, test, fil, func, old, new in RONDAS[ronda]:
        p = os.path.join(out, fil + '.c'); s = open(p, encoding='latin-1', newline='').read()
        crlf = '\r\n' in s
        m = re.search(r'\n' + re.escape(func) + r' \(', s); assert m, (func, fil)
        end = s.index('\n}', m.start()); body = s[m.start():end]
        old2 = old.replace('\n', '\r\n') if crlf else old; new2 = new.replace('\n', '\r\n') if crlf else new
        assert old2 in body, ('no encontrado', k, test, old)
        body = body.replace(old2, new2, 1); s = s[:m.start()] + body + s[end:]
        open(p, 'w', encoding='latin-1', newline='').write(s)
    print('ronda', ronda, ':', len(RONDAS[ronda]), 'mutaciones en', out)

if __name__ == '__main__': main()
