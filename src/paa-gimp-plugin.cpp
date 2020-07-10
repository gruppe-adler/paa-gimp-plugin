#include "paa-gimp-plugin.h"

MAIN()

static void query(void)
{
        static const GimpParamDef loadArgs[] =
        {
                { GIMP_PDB_INT32, "run-mode", "Run mode"},
                { GIMP_PDB_STRING, "filename", "File to load"},
                { GIMP_PDB_STRING, "raw-filename", "Entered name"}
        };

        static const GimpParamDef loadReturnsValues[] =
        {
                { GIMP_PDB_IMAGE, "image", "Output image"}
        };

        static const GimpParamDef saveArgs[] =
        {
                { GIMP_PDB_INT32, "run-mode", "Run mode"},
                { GIMP_PDB_IMAGE, "image", "Input image"},
                { GIMP_PDB_DRAWABLE, "drawable", "The drawable to save"},
                { GIMP_PDB_STRING, "filename", "File to save"},
                { GIMP_PDB_STRING, "raw-filename", "Entered name"}
        };

        gimp_install_procedure(LOAD_PROC,
                           "Loads PAA Images",
                           "Loads PAA Images",
                           "Gruppe Adler",
                           "Gruppe Adler",
                           "2020",
                           "PAA Image",
                           NULL,
                           GIMP_PLUGIN,
                           G_N_ELEMENTS(loadArgs),
                           G_N_ELEMENTS(loadReturnsValues),
                           loadArgs,
                           loadReturnsValues);

        gimp_register_load_handler(LOAD_PROC, "paa", "");
        gimp_register_magic_load_handler(LOAD_PROC, "paa", "", "0,leshort,65281,0,leshort,65285");

        gimp_install_procedure(SAVE_PROC,
                           "Saves PAA Images",
                           "Saves PAA Images",
                           "Gruppe Adler",
                           "Gruppe Adler",
                           "2020",
                           "PAA Image",
                           "RGB*",
                           GIMP_PLUGIN,
                           G_N_ELEMENTS(saveArgs),
                           0,
                           saveArgs,
                           NULL);

        gimp_register_save_handler(SAVE_PROC, "paa", "");
}

static void run(const gchar* name, gint nParams, const GimpParam* param, gint* nReturnValues, GimpParam** returnValues)
{
    static GimpParam values[2];
    GimpPDBStatusType status = GIMP_PDB_SUCCESS;

    *returnValues = values;
    *nReturnValues = 1;

    gimp_ui_init(PLUGIN_NAME, true);
    GimpRunMode runMode = (GimpRunMode)param[0].data.d_int32;

    // force dialog box
    gimp_message_set_handler(GimpMessageHandlerType::GIMP_MESSAGE_BOX);

    if(strcmp (name, LOAD_PROC) == 0) {
        if(nParams != 3)
            status = GIMP_PDB_CALLING_ERROR;

        auto filename = param[1].data.d_string;
        int isInteractive = (runMode == GIMP_RUN_INTERACTIVE);

        if(status == GIMP_PDB_SUCCESS) {
            gint32 gimpImageId = loadPaa(filename, isInteractive);

            if(gimpImageId >= 0) {
                *nReturnValues = 2;
                values[1].type = GIMP_PDB_IMAGE;
                values[1].data.d_image = gimpImageId;
            } else if(gimpImageId == LOAD_PAA_CANCEL) {
                status = GIMP_PDB_CANCEL;
            } else {
                status = GIMP_PDB_EXECUTION_ERROR;
            }
        }
    } else if(strcmp (name, SAVE_PROC) == 0) {
        gint32 imageId;
        gint32 drawableId;

        imageId = param[1].data.d_int32;
        drawableId = param[2].data.d_int32;

        GimpExportReturn _export = GIMP_EXPORT_CANCEL;

        switch (runMode)
        {
            case GIMP_RUN_INTERACTIVE:
            case GIMP_RUN_WITH_LAST_VALS:
                _export = gimp_export_image(&imageId, &drawableId, "PAA", (GimpExportCapabilities)(GIMP_EXPORT_CAN_HANDLE_RGB | GIMP_EXPORT_CAN_HANDLE_ALPHA));

                if(_export == GIMP_EXPORT_CANCEL) {
                    values[0].data.d_status = GIMP_PDB_CANCEL;
                    return;
                }
                break;

            default:
                break;
        }

        if(status == GIMP_PDB_SUCCESS) {
            GError* error = NULL;
            auto result = savePaa(param[3].data.d_string, imageId, drawableId, &error);
            if(!result) {
                status = GIMP_PDB_CALLING_ERROR;
            }
        }
    } else {
        status = GIMP_PDB_CALLING_ERROR;
    }

    values[0].type = GIMP_PDB_STATUS;
    values[0].data.d_status = status;
}


gint32 loadPaa(const gchar* filename, int interactive) {

    auto paa = grad_aff::Paa();
    try {
        paa.readPaa(filename);
    } catch(std::runtime_error& ex) {
        gimp_message((std::string("Exception during Paa Import: \n") + ex.what()).c_str());
        return LOAD_PAA_ERROR;
    }

    auto width = paa.mipMaps[0].width;
    auto height = paa.mipMaps[0].height;
    auto hasAlpha = paa.hasTransparency;

    gint32 imageId = gimp_image_new(width, height, GIMP_RGB);
    gimp_image_set_filename(imageId, filename);

    gint32 layerId = gimp_layer_new(imageId, filename,
                                   width, height,
                                   hasAlpha ? GIMP_RGBA_IMAGE : GIMP_RGB_IMAGE,
                                   100.0, GIMP_NORMAL_MODE);

    gboolean success = gimp_image_insert_layer(imageId, layerId, 0, 0);

    if (!success) {
        gimp_image_delete(imageId);
        return LOAD_PAA_ERROR;
    }

    GimpDrawable *drawable = gimp_drawable_get(layerId);

    auto data = paa.mipMaps[0].data;

    // Remove alpha bytes
    if(!hasAlpha) {
        auto temp = std::vector<uint8_t>();
        temp.reserve((size_t)width * (size_t)height * 3);
        for(size_t i = 0; i < data.size(); i+= 4) {
            temp.push_back(data[i]);
            temp.push_back(data[i + 1]);
            temp.push_back(data[i + 2]);
        }
        data.clear();
        data = temp;
    }

    GimpPixelRgn rgnOut;
    gimp_pixel_rgn_init(&rgnOut, drawable, 0, 0, width, height, true, true);
    gimp_pixel_rgn_set_rect(&rgnOut, data.data(), 0, 0, width, height);

    gimp_drawable_flush (drawable);
    gimp_drawable_merge_shadow (drawable->drawable_id, TRUE);
    gimp_drawable_update (drawable->drawable_id, 0,0, width, height);

    gimp_drawable_detach(drawable);
    return imageId;

}

static bool isPowerOfTwo(uint32_t x) {
	return (x != 0) && ((x & (x - 1)) == 0);
}

gboolean savePaa (const gchar *filename, gint32 imageId, gint32 drawableId, GError **error) {
    auto width = gimp_drawable_width(drawableId);
    auto height = gimp_drawable_height(drawableId);
    int channelNumber = gimp_drawable_bpp(drawableId);

	if (!isPowerOfTwo(width) || !isPowerOfTwo(height)) {
		gimp_message("Error during Paa Export:\nDimensions have to be a power of two (2^n)");
		return false;
	}

    GimpPixelRgn rgnIn;
    auto drawable = gimp_drawable_get(drawableId);

    gimp_pixel_rgn_init(&rgnIn, drawable, 0, 0, width, height, false, false);

    auto data = std::vector<uint8_t>((size_t)width * (size_t)height * channelNumber);
    gimp_pixel_rgn_get_rect(&rgnIn, data.data(), 0, 0, width, height);
    gimp_drawable_detach(drawable);

    // Insert alpha bytes
    if(channelNumber < 4) {
        auto temp = std::vector<uint8_t>();
        temp.reserve((size_t)width * (size_t)height * 4);
        for(size_t i = 0; i < data.size(); i+= 3) {
            temp.push_back(data[i]);
            temp.push_back(data[i + 1]);
            temp.push_back(data[i + 2]);
            temp.push_back(255);
        }
        data.clear();
        data = temp;
    }

    try {
        auto paa = grad_aff::Paa();
        MipMap mipMap;
        mipMap.height = height;
        mipMap.width = width;
        mipMap.data = data;
        mipMap.dataLength = mipMap.data.size();
        paa.mipMaps.clear();
        paa.mipMaps.push_back(mipMap);
        paa.calculateMipmapsAndTaggs();
        paa.writePaa(filename);
    } catch(std::runtime_error& ex) {
        gimp_message((std::string("Exception during Paa Export: \n") + ex.what()).c_str());
        return false;
    }
    return true;
}