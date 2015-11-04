/****************************************************************************
 *
 *   Copyright (c) 2013 Estimation and Control Library (ECL). All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name ECL nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file ecl_roll_controller.cpp
 * Implementation of a simple orthogonal roll PID controller.
 *
 * Authors and acknowledgements in header.
 */

#include <ecl/ecl.h>
#include "ecl_anfis_roll_controller.h"
#include <stdint.h>
#include <float.h>
#include <geo/geo.h>
#include <ecl/ecl.h>
#include <mathlib/mathlib.h>
#include <systemlib/err.h>

ECL_AnfisRollController::ECL_AnfisRollController() :
    ECL_AnfisController("roll")
{
    anfis = (ANFIS_T *) malloc (sizeof(ANFIS_T));

   start_anfis(2,5, PX4_ROOTFSDIR"/etc/params/paramRoll.fin", anfis);
}

ECL_AnfisRollController::~ECL_AnfisRollController()
{
    free(anfis);
}

float ECL_AnfisRollController::control_attitude(const ECL_AnfisControlData &ctl_data)
{
	/* Do not calculate control signal with bad inputs */
	if (!(PX4_ISFINITE(ctl_data.roll_setpoint) && PX4_ISFINITE(ctl_data.roll))) {
		perf_count(_nonfinite_input_perf);
		return _rate_setpoint;
	}

	/* Calculate error */
	float roll_error = ctl_data.roll_setpoint - ctl_data.roll;

	/* Apply P controller */
	_rate_setpoint = roll_error / _tc;

	/* limit the rate */ //XXX: move to body angluar rates

	if (_max_rate > 0.01f) {
		_rate_setpoint = (_rate_setpoint > _max_rate) ? _max_rate : _rate_setpoint;
		_rate_setpoint = (_rate_setpoint < -_max_rate) ? -_max_rate : _rate_setpoint;
	}

	return _rate_setpoint;
}

float ECL_AnfisRollController::control_bodyrate(const ECL_AnfisControlData &ctl_data)
{
	/* Do not calculate control signal with bad inputs */
	if (!(PX4_ISFINITE(ctl_data.pitch) &&
	      PX4_ISFINITE(ctl_data.roll_rate) &&
	      PX4_ISFINITE(ctl_data.yaw_rate) &&
	      PX4_ISFINITE(ctl_data.yaw_rate_setpoint) &&
	      PX4_ISFINITE(ctl_data.airspeed_min) &&
	      PX4_ISFINITE(ctl_data.airspeed_max) &&
	      PX4_ISFINITE(ctl_data.scaler))) {
		perf_count(_nonfinite_input_perf);
		return math::constrain(_last_output, -1.0f, 1.0f);
	}

	/* get the usual dt estimate */
    //uint64_t dt_micros = ecl_elapsed_time(&_last_run);
    //_last_run = ecl_absolute_time();
    //float dt = (float)dt_micros * 1e-6f;

	/* Transform setpoint to body angular rates (jacobian) */
	_bodyrate_setpoint = _rate_setpoint - sinf(ctl_data.pitch) * ctl_data.yaw_rate_setpoint;

	/* Calculate body angular rate error */
	_rate_error = _bodyrate_setpoint - ctl_data.roll_rate; //body angular rate error
    _dif_rate_error = _rate_error - _last_rate_error;
    _last_rate_error = _rate_error;
    double data[2] = {_rate_error, _dif_rate_error};
    run(data, anfis);

	/* Apply PI rate controller and store non-limited output */

	return math::constrain(_last_output, -1.0f, 1.0f);
}

