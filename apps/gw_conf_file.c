/*
 * gw_conf_file.c
 *
 *  Created on: 2013年9月3日
 *      Author: tommy
 */

/*
 * 配置文件格式
 *
 * 				headlen(4B)
 * 				TLVlen(4B)
 * 				TLV
 * 				TLV
 * 				...
 *
 */

#include "gw_conf_file.h"
#include "gwdonuif_interval.h"


typedef struct{
	gw_node node;
	gw_int32 type;
	func_conf_save func_s;
	func_conf_restore func_r;
}gw_conf_handle_t;

typedef struct gw_conf_file_s{
	gw_list handle_list;
	gw_uint32 offset;
	gw_int32 vf;
	gw_int32 isfs;
	gw_uint8 *pval;
	gw_uint32 len;
	gw_int32 (*open)(struct gw_conf_file_s *pf, gw_uint8 * name, gw_int32 mode);
	gw_int32 (*read)(struct gw_conf_file_s *pf, gw_uint32 offset, gw_uint32 len, gw_uint8 *val);
	gw_int32 (*write)(struct gw_conf_file_s *pf, gw_uint32 offset, gw_uint32 len, gw_uint8 *val);
	gw_int32 (*close)(struct gw_conf_file_s *pf);
}gw_conf_file_t;

gw_int32 vfile_open(struct gw_conf_file_s *pf,gw_uint8 * name, gw_int32 mode);
gw_int32 vfile_read(struct gw_conf_file_s *pf, gw_uint32 offset, gw_uint32 len, gw_uint8 *val);
gw_int32 vfile_write(struct gw_conf_file_s *pf, gw_uint32 offset, gw_uint32 len, gw_uint8 *val);
gw_int32 vfile_close(struct gw_conf_file_s *pf);

gw_conf_file_t g_conf_file;

gw_int32 vfile_open(struct gw_conf_file_s *pf, gw_uint8 *name, gw_int32 mode)
{
	gw_int32 ret = -1;
	gw_int32 fd = -1;
	gw_uint8 * pv = NULL;

	ret = call_gwdonu_if_api(LIB_IF_VFILE_OPEN, 4, name, mode, &fd, &pv);

	if(ret >= 0)
	{
		pf->isfs = ret;
		if(ret == 1)
		{
			pf->vf = fd;
		}
		else
		{
			pf->len = fd;
			pf->pval = pv;
		}

		ret = 0;
	}

	return ret;
}

gw_int32 vfile_read(struct gw_conf_file_s *pf, gw_uint32 offset, gw_uint32 len, gw_uint8 *val)
{
	gw_int32 ret = -1;

	if(pf->isfs > 0)
	{
		lseek(pf->vf, offset, SEEK_SET);
		ret = read(pf->vf, val, len);
	}
	else
	{
		if(pf->len >= len + offset)
		{
			memcpy(val, pf->pval+offset, len);
			ret = len;
		}
		else
			ret = 0;
	}

	return ret;
}

gw_int32 vfile_write(struct gw_conf_file_s *pf, gw_uint32 offset, gw_uint32 len, gw_uint8 *val)
{
	gw_int32 ret = -1;

	if(pf->isfs > 0)
	{
		lseek(pf->vf, offset, SEEK_SET);
		ret = write(pf->vf, val, len);
	}
	else
	{
		if(pf->len >= len+offset)
		{
			memcpy(pf->pval+offset, val, len);
			ret = len;
		}
		else
			ret = 0;
	}

	return ret;
}

gw_int32 vfile_close(struct gw_conf_file_s *pf)
{
	gw_int32 ret = -1;

	if(pf->isfs > 0)
		ret = call_gwdonu_if_api(LIB_IF_VFILE_CLOSE, 1, &pf->vf);
	else
		ret = call_gwdonu_if_api(LIB_IF_VFILE_CLOSE, 1, pf->pval);

	pf->isfs  = -1;
	pf->len = 0;
	pf->pval = NULL;

	return ret;
}

static gw_int32 gw_conf_compare(void *data, gw_uint32 key)
{
	gw_int32 ret = 0;

	gw_conf_handle_t * ph = (gw_conf_handle_t*)data;

	if(ph->type > key)
		ret = 1;
	else if(ph->type == key)
		ret = 0;
	else
		ret = -1;

	return ret;

}

gw_int32 gw_register_conf_handlers(gw_int32 type, func_conf_save s, func_conf_restore r)
{
	gw_conf_handle_t *ph = malloc(sizeof(gw_conf_handle_t));

	if(ph)
	{
		ph->type = type;
		ph->func_r = r;
		ph->func_s = s;

		if(gw_lst_find(&g_conf_file.handle_list, type) != NULL)
			return GW_E_CONFLICT;

		printf("%s  type %d\n", __func__, type);

		gw_lst_add(&g_conf_file.handle_list, (gw_node*)ph);

		return GW_OK;
	}

	return GW_ERROR;
}

gw_int32 gw_conf_file_init()
{
	gw_lst_init(&g_conf_file.handle_list, gw_conf_compare);
	g_conf_file.offset = 0;
	g_conf_file.open = vfile_open;
	g_conf_file.read = vfile_read;
	g_conf_file.write = vfile_write;
	g_conf_file.close = vfile_close;

	g_conf_file.isfs = -1;
	g_conf_file.len = 0;
	g_conf_file.pval = NULL;

	return 0;
}

gw_int32 gw_conf_save()
{

	gw_int32 ret = -1, iv = 0;

	gw_conf_file_t * pf = &g_conf_file;

	gw_conf_handle_t *p = NULL;

	if(pf->open(pf, NULL, 2) < 0) //write file
		return ret;

	pf->offset = 8; //headlen and tlvlen

	gw_lst_scan(&(pf->handle_list), p, gw_conf_handle_t *)
	{
		printf("%s    handle type %d\n", __func__, p->type);
		if( p->func_s )
		{
			gw_int32 len = 0;
			gw_uint8 * pv = NULL;
			if(p->func_s(&len, &pv) == GW_OK)
			{
				if(!pf->isfs > 0 && pf->offset+len > pf->len) //not fs surpported and the data exceed the buffer limit
					break;
				pf->write(pf, pf->offset, 4, (gw_uint8*)&p->type);
				pf->offset += 4;
				pf->write(pf, pf->offset, 4, (gw_uint8*)&len);
				pf->offset += 4;
				pf->write(pf, pf->offset, len, pv);
				pf->offset += len;
			}

			if(pv)
				free(pv);
		}
	}

	iv = 8;
	pf->write(pf, 0, 4, (gw_uint8*)&iv);
	iv = pf->offset-8;
	pf->write(pf, 4, 4, (gw_uint8*)&iv);

	pf->close(pf);

	ret = 0;

	return ret;
}

gw_int32 gw_conf_restore()
{
	gw_int32 ret = -1, iv, type, length;

	gw_conf_file_t *pf = &g_conf_file;
	gw_conf_handle_t *p = NULL;

	if(pf->open(pf, NULL, 1) < 0)
		return ret;

	printf("%s  open vfile ok\n", __func__);

	if(pf->isfs > 0)
		pf->len = lseek(pf->vf, 0, SEEK_END);

	pf->offset = 0;

	pf->read(pf, 0, 4, (gw_uint8*)&iv); //get headlen

	pf->offset += iv;

	while(pf->offset < pf->len)
	{
		pf->read(pf, pf->offset, 4, (gw_uint8*)&type);
		pf->offset+=4;
		pf->read(pf, pf->offset, 4, (gw_uint8*)&length);
		pf->offset+=4;

		if(pf->offset + length > pf->len)
			break;

		if((p = (gw_conf_handle_t*)gw_lst_find(&pf->handle_list, type)) != NULL)
		{
			gw_uint8 * data = malloc(length);
			if(data)
			{
				pf->read(pf, pf->offset, length, data);
				p->func_r(length, data);
				free(data);
			}
		}

		pf->offset += length;
	}

	pf->close(pf);

	ret = 0;

	return ret;
}
