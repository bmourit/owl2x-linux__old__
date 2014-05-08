/*
 * Copyright (C) 2013 Red Hat
 * Author: Rob Clark <robdclark@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "vivante_drv.h"
#include "vivante_gem.h"

struct sg_table *vivante_gem_prime_get_sg_table(struct drm_gem_object *obj)
{
	struct vivante_gem_object *vivante_obj = to_vivante_bo(obj);
	BUG_ON(!vivante_obj->sgt);  /* should have already pinned! */
	return vivante_obj->sgt;
}

void *vivante_gem_prime_vmap(struct drm_gem_object *obj)
{
	return vivante_gem_vaddr(obj);
}

void vivante_gem_prime_vunmap(struct drm_gem_object *obj, void *vaddr)
{
	/* TODO vivante_gem_vunmap() */
}

struct drm_gem_object *vivante_gem_prime_import_sg_table(struct drm_device *dev,
		size_t size, struct sg_table *sg)
{
	return vivante_gem_import(dev, size, sg);
}

int vivante_gem_prime_pin(struct drm_gem_object *obj)
{
	if (!obj->import_attach)
		vivante_gem_get_pages(obj);
	return 0;
}

void vivante_gem_prime_unpin(struct drm_gem_object *obj)
{
	if (!obj->import_attach)
		vivante_gem_put_pages(obj);
}
