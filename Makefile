CC      := gcc
CFLAGS  := -Wall -Wextra -std=c99
LDFLAGS :=
LDLIBS  := -lgdi32 -luser32 -lkernel32

TARGET  := app.exe
SRCDIR  := src
OBJDIR  := build
SRCS    := $(wildcard $(SRCDIR)/*.c)
OBJS    := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

.PHONY: all clean

all: $(TARGET)

$(OBJDIR):
ifeq ($(OS),Windows_NT)
	cmd /C "if not exist $(OBJDIR) mkdir $(OBJDIR)"
else
	mkdir -p $(OBJDIR)
endif

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
ifeq ($(OS),Windows_NT)
	cmd /C "if exist $(OBJDIR)\*.o del /F /Q $(OBJDIR)\*.o"
else
	$(RM) $(OBJS)
endif

clean:
ifeq ($(OS),Windows_NT)
	cmd /C "if exist $(TARGET) del /F /Q $(TARGET)"
	cmd /C "if exist $(OBJDIR) rmdir /S /Q $(OBJDIR)"
else
	$(RM) $(TARGET)
	rm -rf $(OBJDIR)
endif
