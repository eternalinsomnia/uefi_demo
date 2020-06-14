#include <efi.h>
#include <efilib.h>

EFI_GRAPHICS_OUTPUT_BLT_PIXEL *screen;
EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput;
EFI_HANDLE *HandleBuffer;
EFI_INPUT_KEY key;
EFI_EVENT key_event;
UINTN index_;
UINTN HandleCount;

UINTN field[20][25];
UINTN width = 20;
UINTN height = 25;

UINTN screen_x;
UINTN screen_y;
UINTN screen_width = 400;
UINTN screen_height = 500;

UINTN background = 0;

UINTN border_width = 5;
UINTN border_color = 1;

UINTN block_width = 20;

UINTN tetronimo_type = 6;
UINTN tetronimo_color = 1;

INTN dx = 0;
INTN dy = 0;

UINTN tetronimo[7][4] = 
{
	{ 1,3,5,7 },
	{ 2,4,5,7 },
	{ 3,5,4,6 },
	{ 3,5,4,7 },
	{ 2,3,5,7 },
	{ 3,5,7,6 },
	{ 2,3,4,5 },
};

UINT8 color[9][3] = 
{
	{ 0x00,0x00,0x00 },
	{ 0xff,0xff,0xff },
	{ 0x88,0x4e,0xa0 }, 
	{ 0xa9,0x32,0x26 }, 
	{ 0x24,0x71,0xa3 }, 
	{ 0x17,0xa5,0x89 }, 
	{ 0xf1,0xc4,0x0f },
	{ 0x70,0x7b,0x7c },
	{ 0x34,0x49,0x5e }
};

struct Point
{
	UINTN x, y;
} point[4], temp[4];

VOID clear_field()
{
	for(UINTN i = 0; i < height; i++)
	{
		for(UINTN j = 0; j < width; j++) field[j][i] = 0;
	}
}

VOID render()
{
	GraphicsOutput->Blt(GraphicsOutput, screen, EfiBltBufferToVideo, 0, 0, screen_x, screen_y, screen_width, screen_height, 0);
}

VOID clear_screen()
{
	for(UINTN i = 0; i < screen_width * screen_height; i++)
	{
		screen[i].Red = color[background][0];
		screen[i].Green = color[background][1];
		screen[i].Blue = color[background][2];
 		screen[i].Reserved = 0x00;
	}
}

VOID draw_border()
{
	UINTN size = (screen_width + border_width * 2) * (screen_height + border_width * 2);
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL border[size];
	
	for(UINTN i = 0; i < size; i++)
	{
		border[i].Blue = color[border_color][0];
		border[i].Green = color[border_color][1];
		border[i].Red = color[border_color][2];
		border[i].Reserved = 0x00;
	}

	GraphicsOutput->Blt(GraphicsOutput, border, EfiBltBufferToVideo, 0, 0,
		screen_x - border_width, screen_y - border_width,screen_width + border_width * 2, screen_height + border_width * 2, 0);
}

VOID draw_block(UINTN x, UINTN y, UINTN block_color)
{
	UINTN pos = 0;
	
	for(UINTN i = 0; i < block_width; i++)
	{
		for(UINTN j = 0; j < block_width; j++)
		{
			pos = (y * block_width + i) * screen_width + (x * block_width + j);

			screen[pos].Red = color[block_color][0];
			screen[pos].Green = color[block_color][1];
			screen[pos].Blue = color[block_color][2];
			screen[pos].Reserved = 0x00;
		}
	}
}

UINTN collision()
{
	for(UINTN i = 0; i < 4; i++)
	{
		if((point[i].x >= width || point[i].y >= height) 
			|| (field[point[i].x][point[i].y])) return 1;	
	}
	return 0;
}

VOID restore_previous()
{
	for(UINTN i = 0; i < 4; i++) point[i] = temp[i];
}

VOID generate_tetronimo()
{
	for(UINTN i = 0; i < 4; i++)
	{
		point[i].x = tetronimo[tetronimo_type][i] % 2 + screen_x / block_width - 1;
		point[i].y = tetronimo[tetronimo_type][i] / 2;
	}

	if(collision()) clear_field();
}

VOID draw_tetromino()
{
	for(UINTN i = 0; i < height; i++)
	{
		for(UINTN j = 0; j < width; j++)
		{
			if(field[j][i] == 0) continue;
			draw_block(j, i, field[j][i]);
		}
	}

	for(UINTN i = 0; i < 4; i++) draw_block(point[i].x, point[i].y, tetronimo_color);
}

VOID check_line()
{
	UINTN k = height - 1;
	
	for(UINTN i = height - 1; i > 0; i--)
	{
		UINTN count = 0;
		for(UINTN j = 0; j < width; j++)
		{
			if(field[j][i]) count++;
			field[j][k] = field[j][i];
		}
		if(count < width) k--;
	}
}

VOID move(INTN x, INTN y)
{
	dx = x;
	dy = y;
	
	for(UINTN i = 0; i < 4; i++)
	{
		temp[i] = point[i];
		point[i].x += dx;
		point[i].y += dy;
	}

	if(dy)
	{
		if(collision())
		{
			for(UINTN i = 0; i < 4; i++) field[temp[i].x][temp[i].y] = tetronimo_color;
			generate_tetronimo();

			tetronimo_color++;
			if(tetronimo_color == 9) tetronimo_color = 1;
		}	
	}
	
	if(dx)
	{
		if(collision()) restore_previous();
	}
}

VOID rotate()
{
	struct Point center = point[1];
	for(UINTN i = 0; i < 4; i++)
	{
		UINTN temp_x = point[i].y - center.y;
		UINTN temp_y = point[i].x - center.x;
		point[i].x = center.x - temp_x;
		point[i].y = center.y + temp_y;
	}

	if(collision()) restore_previous();
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	InitializeLib(ImageHandle, SystemTable);
	gST->ConOut->ClearScreen(gST->ConOut);
	
	gBS->AllocatePool(EfiBootServicesData, screen_width*screen_height * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL), (VOID **)&screen);
	
	EFI_STATUS Status = gST->BootServices->LocateHandleBuffer(ByProtocol, &gEfiGraphicsOutputProtocolGuid, NULL, &HandleCount, &HandleBuffer);
	if(EFI_ERROR(Status)) return EFI_UNSUPPORTED;

	for (UINTN i = 0; i < HandleCount; i++)
	{
		gST->BootServices->HandleProtocol(HandleBuffer[i], &gEfiGraphicsOutputProtocolGuid, (void **)&GraphicsOutput);
	}

	screen_x = GraphicsOutput->Mode->Info->HorizontalResolution / 2 - screen_width / 2;
	screen_y = GraphicsOutput->Mode->Info->VerticalResolution / 2 - screen_height / 2;

	draw_border();	
	clear_field();
	generate_tetronimo();

	while(1)
	{
		key_event = gST->ConIn->WaitForKey;
		gBS->WaitForEvent(1, key_event, &index_);
		gST->ConIn->ReadKeyStroke(gST->ConIn, &key);

		if(key.ScanCode == SCAN_RIGHT) move(1 ,0);
		if(key.ScanCode == SCAN_LEFT) move(-1 ,0);
		if(key.ScanCode == SCAN_UP) rotate();
		if(key.ScanCode == SCAN_DOWN) move(0, 1);
		if(key.ScanCode == SCAN_ESC) break;

		draw_tetromino();
		check_line();
		render();
		clear_screen();

		dx = 0;
		dy = 0;
		
		tetronimo_type++;
		if(tetronimo_type == 7) tetronimo_type = 0;
	}

	gBS->FreePool(screen);

	return EFI_SUCCESS;
}