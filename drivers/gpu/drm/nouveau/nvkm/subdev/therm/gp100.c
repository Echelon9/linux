/*
 * Copyright 2017 Rhys Kidd
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors: Rhys Kidd
 */
#include "priv.h"

static int
gp100_temp_get(struct nvkm_therm *therm)
{
	struct nvkm_device *device = therm->subdev.device;
	struct nvkm_subdev *subdev = &therm->subdev;
	uint32_t packed_tsensor;

	packed_tsensor = nvkm_rd32(device, 0x20460);

	/* device SHADOWed */
	if (((packed_tsensor >> 29) & 0x3) == 0x2)
		nvkm_error(subdev, "Reading temperature from SHADOWed sensor\n");

	/* device valid */
	if (((packed_tsensor >> 29) & 0x3) == 0x1)
		return (packed_tsensor & 0x3fff << 3) / 256;
	else
		return -ENODEV;
}

static const struct nvkm_therm_func
gp100_therm = {
	.temp_get = gp100_temp_get,
	.program_alarms = nvkm_therm_program_alarms_polling,
};

int
gp100_therm_new(struct nvkm_device *device, int index,
		struct nvkm_therm **ptherm)
{
	return nvkm_therm_new_(&gp100_therm, device, index, ptherm);
}