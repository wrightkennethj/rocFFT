/*******************************************************************************
 * Copyright (C) 2016 Advanced Micro Devices, Inc. All rights reserved.
 ******************************************************************************/

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "logging.h"
#include "plan.h"
#include "repo.h"
#include "rocfft.h"
#include "transform.h"

rocfft_status rocfft_execution_info_create(rocfft_execution_info* info)
{
    rocfft_execution_info einfo = new rocfft_execution_info_t;
    *info                       = einfo;
    log_trace(__func__, "info", *info);

    return rocfft_status_success;
}

rocfft_status rocfft_execution_info_destroy(rocfft_execution_info info)
{
    log_trace(__func__, "info", info);
    if(info != nullptr)
        delete info;

    return rocfft_status_success;
}

rocfft_status rocfft_execution_info_set_work_buffer(rocfft_execution_info info,
                                                    void*                 work_buffer,
                                                    size_t                work_buffer_size)
{
    log_trace(
        __func__, "info", info, "work_buffer", work_buffer, "work_buffer_size", work_buffer_size);
    info->workBufferSize = work_buffer_size;
    info->workBuffer     = work_buffer;

    return rocfft_status_success;
}

rocfft_status rocfft_execution_info_set_stream(rocfft_execution_info info, void* stream)
{
    log_trace(__func__, "info", info, "stream", stream);
    info->rocfft_stream = (hipStream_t)stream;
    return rocfft_status_success;
}

rocfft_status rocfft_execute(const rocfft_plan     plan,
                             void*                 in_buffer[],
                             void*                 out_buffer[],
                             rocfft_execution_info info)
{
    log_trace(
        __func__, "plan", plan, "in_buffer", in_buffer, "out_buffer", out_buffer, "info", info);

    Repo&    repo = Repo::GetRepo();
    ExecPlan execPlan;
    repo.GetPlan(plan, execPlan);

#ifdef DEBUG
    PrintNode(std::cout, execPlan);
#endif

    if(execPlan.workBufSize > 0)
    {
        assert(info != nullptr);
        assert(info->workBufferSize >= (execPlan.workBufSize * 2 * plan->base_type_size));
    }

    if(plan->placement == rocfft_placement_inplace)
        TransformPowX(execPlan, in_buffer, in_buffer, info);
    else
        TransformPowX(execPlan, in_buffer, out_buffer, info);

    return rocfft_status_success;
}
