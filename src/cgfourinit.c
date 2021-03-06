
cgfour_enter_color()
{
  cgfour_change_screen_mode(COLOR_SCREEN);
}

cgfour_exit_color()
{
}

cgfour_enter_mono()
{
  cgfour_change_screen_mode(MONO_SCREEN);
}

cgfour_exit_mono()
{
}

cgfour_color_init( dsp )
     DspInterface dsp;
{
  struct videoconfig vc;

  dsp->enter_display = &cgfour_enter_color;
  dsp->exit_display = &cgfour_exit_color;

  dsp->before_raid = &VGA_exit;
  dsp->after_raid = &VGA_setmax;
    
  dsp->drawline = &VGA_panic;

  dsp->cleardisplay = &VGA_cleardisplay;

  dsp->get_color_map_entry = &VGA_not_color;
  dsp->set_color_map_entry = &VGA_not_color;
  dsp->available_colors = &VGA_not_color;
  dsp->possible_colors = &VGA_not_color;

  dsp->medley_to_native_bm = &VGA_panic;
  dsp->native_to_medley_bm = &VGA_panic;

  dsp->bitblt_to_screen = &DosbbtC1;
  dsp->bitblt_from_screen = &VGA_panic;
  dsp->scroll_region = &VGA_panic;

  VGA_setmax( dsp );
}

cgfour_mono_init( dsp )
     DspInterface dsp;
{
  struct videoconfig vc;

  dsp->enter_display = &cgfour_enter_color;
  dsp->exit_display = &cgfour_exit_color;

  dsp->before_raid = &VGA_exit;
  dsp->after_raid = &VGA_setmax;
    
  dsp->drawline = &VGA_panic;

  dsp->cleardisplay = &VGA_cleardisplay;

  dsp->get_color_map_entry = &VGA_not_color;
  dsp->set_color_map_entry = &VGA_not_color;
  dsp->available_colors = &VGA_not_color;
  dsp->possible_colors = &VGA_not_color;

  dsp->medley_to_native_bm = &VGA_panic;
  dsp->native_to_medley_bm = &VGA_panic;

  dsp->bitblt_to_screen = &DosbbtC1;
  dsp->bitblt_from_screen = &VGA_panic;
  dsp->scroll_region = &VGA_panic;

  VGA_setmax( dsp );
}
