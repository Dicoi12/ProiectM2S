c-common.c:		  char *newp = (char *) alloca (len - 1);
c-decl.c:  builtin_function ("__builtin_alloca",
c-decl.c:		    BUILT_IN_ALLOCA, "alloca");
c-decl.c:      temp = builtin_function ("alloca",
c-lex.c:		char *copy = (char *) alloca (p - token_buffer + 1);
c-lex.c:	    char *widep = (char *) alloca ((p - token_buffer) * WCHAR_BYTES);
c-parse.c:#ifndef alloca
c-parse.c:#define alloca __builtin_alloca
c-parse.c:#include <alloca.h>
c-parse.c: #pragma alloca
c-parse.c:void *alloca (unsigned int);
c-parse.c:void *alloca ();
c-parse.c:#endif /* alloca not defined.  */
c-parse.c:      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
c-parse.c:      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
c-parse.c:      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
c-typeck.c:   address of it; it should not be allocated in a register.
c-typeck.c:	  opname = (char *) alloca (IDENTIFIER_LENGTH (function)
c-typeck.c:	  opname = (char *) alloca (sizeof (argnofun) + 25 /*%d*/ + 1);
c-typeck.c:      temp = (char *) alloca (needed);
c-typeck.c:    ofwhat = print_spelling ((char *) alloca (spelling_length () + 1));
c-typeck.c:  buffer = (char *) alloca (strlen (local) + strlen (ofwhat) + 2);
c-typeck.c:    ofwhat = print_spelling ((char *) alloca (spelling_length () + 1));
c-typeck.c:  buffer = (char *) alloca (strlen (local) + strlen (ofwhat) + 2);
c-typeck.c:    ofwhat = print_spelling ((char *) alloca (spelling_length () + 1));
c-typeck.c:  buffer = (char *) alloca (strlen (local) + strlen (ofwhat) + 2);
c-typeck.c:  register tree *o = (tree *) alloca (noutputs * sizeof (tree));
calls.c:   `alloca'.
calls.c:  /* We assume that alloca will always be called by name.  It
calls.c:  may_be_alloca =
calls.c:		 && ! strcmp (name, "alloca"))
calls.c:		    && ! strcmp (name, "__builtin_alloca"))));
calls.c:  args = (struct arg_data *) alloca (num_actuals * sizeof (struct arg_data));
calls.c:	  stack_usage_map = (char *) alloca (highest_outgoing_arg_in_use);
calls.c:	  stack_usage_map = (char *)alloca (highest_outgoing_arg_in_use);
calls.c:	args[i].aligned_regs = (rtx *) alloca (sizeof (rtx)
cccp.c:    char *p = (char *) alloca (strlen (predefs) + 1);
cexp.c:#define alloca __builtin_alloca
cexp.c:#include <alloca.h>
cexp.c: #pragma alloca
cexp.c:void *alloca (unsigned int);
cexp.c:void *alloca ();
cexp.c:#endif /* alloca not defined.  */
cexp.c:  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */
cexp.c:      /* Give user a chance to reallocate the stack */
cexp.c:      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
combine.c:    = (unsigned HOST_WIDE_INT *) alloca (nregs * sizeof (HOST_WIDE_INT));
cse.c:      sets = (struct set *) alloca (sizeof (struct set));
dbxout.c:    = (char *) alloca (strlen (cwd) + strlen (rel_filename) + 1);
expmed.c:  best_alg = (struct algorithm *)alloca (sizeof (struct algorithm));
expr.c:  rtx *savevec = (rtx *) alloca (FIRST_PSEUDO_REGISTER * sizeof (rtx));
final.c:	char *data_file = (char *) alloca (len + 4);
flow.c:  basic_block_drops_in = (char *) alloca (n_basic_blocks);
function.c:  block_stack = (int *) alloca (n_blocks * sizeof (int));
gcc.c:      char *nstore = (char *) alloca (strlen (temp) + 3);
global.c:  reg_allocno = (int *) alloca (max_regno * sizeof (int));
integrate.c:  parmdecl_map = (tree *) alloca (max_parm_reg * sizeof (tree));
jump.c:  jump_chain = (rtx *) alloca (max_jump_chain * sizeof (rtx));
local-alloc.c:  qty_phys_reg = (short *) alloca (max_qty * sizeof (short));
local-alloc.c:  rtx *reg_equiv_init_insn = (rtx *) alloca (max_regno * sizeof (rtx *));
loop.c:  moved_once = (char *) alloca (max_reg_before_loop);
m88k.c:        |    [dynamically allocated space (alloca)]    |
real.c:  lstr = (char *) alloca (strlen (ss) + 1);
recog.c:  operands = (rtx *) alloca (noperands * sizeof (rtx));
reg-stack.c:  block_begin = (rtx *) alloca (blocks * sizeof (rtx));
regclass.c:  in_inc_dec = (char *) alloca (nregs);
reload1.c:  reg_equiv_constant = (rtx *) alloca (max_regno * sizeof (rtx));
reorg.c:  uid_to_ruid = (int *) alloca ((max_uid + 1) * sizeof (int *));
rtl.c:		alloca (sizeof (struct rtx_list));
sched.c:  reg_last_uses = (rtx *) alloca (i * sizeof (rtx));
stmt.c:  argvec = (rtx *) alloca (i * sizeof (rtx));
stupid.c:  uid_suid = (int *) alloca ((i + 1) * sizeof (int));
toplev.c:  char *s = (char *) alloca (len);
tree.c:  buf = (char *) alloca (sizeof (FILE_FUNCTION_FORMAT) + strlen (p));
unroll.c:  map = (struct inline_remap *) alloca (sizeof (struct inline_remap));
varasm.c:      p = (char*) alloca (len);
xcoffout.c:	    n = (char *) alloca (i + 1);
