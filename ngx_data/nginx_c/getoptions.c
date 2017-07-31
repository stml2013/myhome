static ngx_int_t ngx_get_options(int argc, char* const *argv)
{
	u_char    *p
	ngx_int_i  i;

	for (i = 1; i < argc; ++i)
	{
		p = (u_char*)argv[i];

		if (*p++ != '-')
		{
            ngx_log_stderr(0, "invalid option \"%s\"", argv[i]);
            return NGX_ERROR;
		}

		while (*p)
		{
            switch(*p++)
            {
            }
		}
	}
}
