typedef struct {
  int		LastCursorX;
  int		LastCursorY;
  int		LastCursorClippingX;
  int		LastCursorClippingY;
  DLword	SaveBitmap;
  DLword	CursorImage;
  void		(* exit_device)();
  void		(* enter_device)();
  void		(* set_cursor_position)();
  void		(* set_cursor_image)();
  void		(* motion_event)();
  void		(* button_event)();
} MouseInterfaceRec *MouseInterface;

