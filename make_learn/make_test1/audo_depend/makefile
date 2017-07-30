
#简化

sources = main.c print.c computer.c


%.d: %.c
	@set -e; rm -f $@; \
$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
rm -f $@.$$$$

include $(soruces:.c=.d)

