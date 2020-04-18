// paa-gimp-plugin.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>
#include <grad_aff/paa/paa.h>

#include <filesystem>
#include <vector>
#include <iostream>

#define PLUGIN_NAME "paa-gimp-plugin"
#define LOAD_PROC "file-paa-load"
#define SAVE_PROC "file-paa-save"
#define LOAD_PAA_ERROR -1
#define LOAD_PAA_CANCEL -2

namespace fs = std::filesystem;

static void query (void);
static void run (const gchar *name, gint nparams, const GimpParam  *param, gint *nreturn_vals, GimpParam **return_vals);

gint32 loadPaa(const gchar* filename, int interactive);
gboolean savePaa (const gchar *filename, gint32 imageId, gint32 drawableId, GError **error);

const GimpPlugInInfo PLUG_IN_INFO =
{
    NULL,
    NULL,
    query,
    run,
};