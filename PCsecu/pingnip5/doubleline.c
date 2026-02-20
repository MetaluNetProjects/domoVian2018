/* Copyright (c) 1997-1999 Miller Puckette.
* For information on usage and redistribution, and for a DISCLAIMER OF ALL
* WARRANTIES, see the file, "LICENSE.txt," in this distribution.  */

#include "m_pd.h"
#include "math.h"

static t_class *line_tilde_class;

typedef struct _line
{
    t_object x_obj;
    double x_target; /* target value of ramp */
    double x_value; /* current value of ramp at block-borders */
    double x_biginc;
    double x_inc;
    double x_1overn;
    double x_dspticktomsec;
    t_float x_inletvalue;
    t_float x_inletwas;
    int x_ticksleft;
    int x_retarget;
} t_line;

static t_int *line_tilde_perform(t_int *w)
{
    t_line *x = (t_line *)(w[1]);
    t_sample *out = (t_sample *)(w[2]);
    int n = (int)(w[3]);
    double f = x->x_value;

    if (PD_BIGORSMALL(f))
            x->x_value = f = 0;
    if (x->x_retarget)
    {
        int nticks = x->x_inletwas * x->x_dspticktomsec;
        if (!nticks) nticks = 1;
        x->x_ticksleft = nticks;
        x->x_biginc = (x->x_target - x->x_value)/(double)nticks;
        x->x_inc = x->x_1overn * x->x_biginc;
        x->x_retarget = 0;
    }
    if (x->x_ticksleft)
    {
        while (n--) *out++ = f, f += x->x_inc;
        x->x_value += x->x_biginc;
        x->x_ticksleft--;
    }
    else
    {
        t_sample g = x->x_value = x->x_target;
        while (n--)
            *out++ = g;
    }
    return (w+4);
}

/* TB: vectorized version */
static t_int *line_tilde_perf8(t_int *w)
{
    t_line *x = (t_line *)(w[1]);
    t_sample *out = (t_sample *)(w[2]);
    int n = (int)(w[3]);
    double f = x->x_value;

    if (PD_BIGORSMALL(f))
        x->x_value = f = 0;
    if (x->x_retarget)
    {
        int nticks = x->x_inletwas * x->x_dspticktomsec;
        if (!nticks) nticks = 1;
        x->x_ticksleft = nticks;
        x->x_biginc = (x->x_target - x->x_value)/(double)nticks;
        x->x_inc = x->x_1overn * x->x_biginc;
        x->x_retarget = 0;
    }
    if (x->x_ticksleft)
    {
        while (n--) *out++ = f, f += x->x_inc;
        x->x_value += x->x_biginc;
        x->x_ticksleft--;
    }
    else
    {
        f = x->x_value = x->x_target;
        for (; n; n -= 8, out += 8)
        {
            out[0] = f; out[1] = f; out[2] = f; out[3] = f;
            out[4] = f; out[5] = f; out[6] = f; out[7] = f;
        }
    }
    return (w+4);
}

static void line_tilde_float(t_line *x, t_float f)
{
    if (x->x_inletvalue <= 0)
    {
        x->x_target = x->x_value = f;
        x->x_ticksleft = x->x_retarget = 0;
    }
    else
    {
        x->x_target = f;
        x->x_retarget = 1;
        x->x_inletwas = x->x_inletvalue;
        x->x_inletvalue = 0;
    }
}

static void line_tilde_stop(t_line *x)
{
    x->x_target = x->x_value;
    x->x_ticksleft = x->x_retarget = 0;
}

static void line_tilde_dsp(t_line *x, t_signal **sp)
{
    if(sp[0]->s_n&7)
        dsp_add(line_tilde_perform, 3, x, sp[0]->s_vec, (t_int)sp[0]->s_n);
    else
        dsp_add(line_tilde_perf8, 3, x, sp[0]->s_vec, (t_int)sp[0]->s_n);
    x->x_1overn = 1./sp[0]->s_n;
    x->x_dspticktomsec = sp[0]->s_sr / ((double)1000 * sp[0]->s_n);
}

static void *line_tilde_new(void)
{
    t_line *x = (t_line *)pd_new(line_tilde_class);
    outlet_new(&x->x_obj, gensym("signal"));
    floatinlet_new(&x->x_obj, &x->x_inletvalue);
    x->x_ticksleft = x->x_retarget = 0;
    x->x_value = x->x_target = x->x_inletvalue = x->x_inletwas = 0;
    return (x);
}

void doubleline_tilde_setup(void)
{
    line_tilde_class = class_new(gensym("doubleline~"), line_tilde_new, 0,
        sizeof(t_line), 0, 0);
    class_addfloat(line_tilde_class, (t_method)line_tilde_float);
    class_addmethod(line_tilde_class, (t_method)line_tilde_dsp,
        gensym("dsp"), A_CANT, 0);
    class_addmethod(line_tilde_class, (t_method)line_tilde_stop,
        gensym("stop"), 0);
}


